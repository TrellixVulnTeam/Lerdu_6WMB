/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/
#include "php.h"
#include "php_globals.h"
#include "mysqlnd.h"
#include "mysqlnd_priv.h"
#include "mysqlnd_wireprotocol.h"
#include "mysqlnd_statistics.h"
#include "mysqlnd_debug.h"
#include "ext/standard/sha1.h"
#include "zend_ini.h"

#define MYSQLND_SILENT 1

#define MYSQLND_DUMP_HEADER_N_BODY

#define	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_size, packet_type_as_text, packet_type) \
	{ \
		DBG_INF_FMT("buf=%p size=%u", (buf), (buf_size)); \
		if (FAIL == mysqlnd_read_header((conn), &((packet)->header) TSRMLS_CC)) {\
			CONN_SET_STATE(conn, CONN_QUIT_SENT); \
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);\
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysqlnd_server_gone); \
			DBG_ERR_FMT("Can't read %s's header", (packet_type_as_text)); \
			DBG_RETURN(FAIL);\
		}\
		if ((buf_size) < (packet)->header.size) { \
			DBG_ERR_FMT("Packet buffer %u wasn't big enough %u, %u bytes will be unread", \
						(buf_size), (packet)->header.size, (packet)->header.size - (buf_size)); \
						DBG_RETURN(FAIL); \
		}\
		if (FAIL == conn->net->m.receive((conn), (buf), (packet)->header.size TSRMLS_CC)) { \
			CONN_SET_STATE(conn, CONN_QUIT_SENT); \
			SET_CLIENT_ERROR(conn->error_info, CR_SERVER_GONE_ERROR, UNKNOWN_SQLSTATE, mysqlnd_server_gone);\
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", mysqlnd_server_gone); \
			DBG_ERR_FMT("Empty '%s' packet body", (packet_type_as_text)); \
			DBG_RETURN(FAIL);\
		} \
		MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn->stats, packet_type_to_statistic_byte_count[packet_type], \
											MYSQLND_HEADER_SIZE + (packet)->header.size, \
											packet_type_to_statistic_packet_count[packet_type], \
											1); \
	}


#define BAIL_IF_NO_MORE_DATA \
	if ((size_t)(p - begin) > packet->header.size) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Premature end of data (mysqlnd_wireprotocol.c:%u)", __LINE__); \
		goto premature_end; \
	} \


static const char *unknown_sqlstate= "HY000";

const char * const mysqlnd_empty_string = "";

/* Used in mysqlnd_debug.c */
const char mysqlnd_read_header_name[]	= "mysqlnd_read_header";
const char mysqlnd_read_body_name[]		= "mysqlnd_read_body";


#define ERROR_MARKER 0xFF
#define EODATA_MARKER 0xFE

/* {{{ mysqlnd_command_to_text
 */
const char * const mysqlnd_command_to_text[COM_END] =
{
  "SLEEP", "QUIT", "INIT_DB", "QUERY", "FIELD_LIST",
  "CREATE_DB", "DROP_DB", "REFRESH", "SHUTDOWN", "STATISTICS",
  "PROCESS_INFO", "CONNECT", "PROCESS_KILL", "DEBUG", "PING",
  "TIME", "DELAYED_INSERT", "CHANGE_USER", "BINLOG_DUMP",
  "TABLE_DUMP", "CONNECT_OUT", "REGISTER_SLAVE",
  "STMT_PREPARE", "STMT_EXECUTE", "STMT_SEND_LONG_DATA", "STMT_CLOSE",
  "STMT_RESET", "SET_OPTION", "STMT_FETCH", "DAEMON"
};
/* }}} */



static enum_mysqlnd_collected_stats packet_type_to_statistic_byte_count[PROT_LAST] =
{
	STAT_LAST,
	STAT_LAST,
	STAT_BYTES_RECEIVED_OK,
	STAT_BYTES_RECEIVED_EOF,
	STAT_LAST,
	STAT_BYTES_RECEIVED_RSET_HEADER,
	STAT_BYTES_RECEIVED_RSET_FIELD_META,
	STAT_BYTES_RECEIVED_RSET_ROW,
	STAT_BYTES_RECEIVED_PREPARE_RESPONSE,
	STAT_BYTES_RECEIVED_CHANGE_USER,
};

static enum_mysqlnd_collected_stats packet_type_to_statistic_packet_count[PROT_LAST] =
{
	STAT_LAST,
	STAT_LAST,
	STAT_PACKETS_RECEIVED_OK,
	STAT_PACKETS_RECEIVED_EOF,
	STAT_LAST,
	STAT_PACKETS_RECEIVED_RSET_HEADER,
	STAT_PACKETS_RECEIVED_RSET_FIELD_META,
	STAT_PACKETS_RECEIVED_RSET_ROW,
	STAT_PACKETS_RECEIVED_PREPARE_RESPONSE,
	STAT_PACKETS_RECEIVED_CHANGE_USER,
};


/* {{{ php_mysqlnd_net_field_length
   Get next field's length */
unsigned long
php_mysqlnd_net_field_length(zend_uchar **packet)
{
	register zend_uchar *p= (zend_uchar *)*packet;

	if (*p < 251) {
		(*packet)++;
		return (unsigned long) *p;
	}

	switch (*p) {
		case 251:
			(*packet)++;
			return MYSQLND_NULL_LENGTH;
		case 252:
			(*packet) += 3;
			return (unsigned long) uint2korr(p+1);
		case 253:
			(*packet) += 4;
			return (unsigned long) uint3korr(p+1);
		default:
			(*packet) += 9;
			return (unsigned long) uint4korr(p+1);
	}
}
/* }}} */


/* {{{ php_mysqlnd_net_field_length_ll
   Get next field's length */
uint64_t
php_mysqlnd_net_field_length_ll(zend_uchar **packet)
{
	register zend_uchar *p= (zend_uchar *)*packet;

	if (*p < 251) {
		(*packet)++;
		return (uint64_t) *p;
	}

	switch (*p) {
		case 251:
			(*packet)++;
			return (uint64_t) MYSQLND_NULL_LENGTH;
		case 252:
			(*packet) += 3;
			return (uint64_t) uint2korr(p + 1);
		case 253:
			(*packet) += 4;
			return (uint64_t) uint3korr(p + 1);
		default:
			(*packet) += 9;
			return (uint64_t) uint8korr(p + 1);
	}
}
/* }}} */


/* {{{ php_mysqlnd_net_store_length */
zend_uchar *
php_mysqlnd_net_store_length(zend_uchar *packet, uint64_t length)
{
	if (length < (uint64_t) L64(251)) {
		*packet = (zend_uchar) length;
		return packet + 1;
	}

	if (length < (uint64_t) L64(65536)) {
		*packet++ = 252;
		int2store(packet,(unsigned int) length);
		return packet + 2;
	}

	if (length < (uint64_t) L64(16777216)) {
		*packet++ = 253;
		int3store(packet,(ulong) length);
		return packet + 3;
	}
	*packet++ = 254;
	int8store(packet, length);
	return packet + 8;
}
/* }}} */


/* {{{ php_mysqlnd_read_error_from_line */
static enum_func_status
php_mysqlnd_read_error_from_line(zend_uchar *buf, size_t buf_len,
								char *error, int error_buf_len,
								unsigned int *error_no, char *sqlstate TSRMLS_DC)
{
	zend_uchar *p = buf;
	int error_msg_len= 0;

	DBG_ENTER("php_mysqlnd_read_error_from_line");

	*error_no = CR_UNKNOWN_ERROR;
	memcpy(sqlstate, unknown_sqlstate, MYSQLND_SQLSTATE_LENGTH);

	if (buf_len > 2) {
		*error_no = uint2korr(p);
		p+= 2;
		/*
		  sqlstate is following. No need to check for buf_left_len as we checked > 2 above,
		  if it was >=2 then we would need a check
		*/
		if (*p == '#') {
			++p;
			if ((buf_len - (p - buf)) >= MYSQLND_SQLSTATE_LENGTH) {
				memcpy(sqlstate, p, MYSQLND_SQLSTATE_LENGTH);
				p+= MYSQLND_SQLSTATE_LENGTH;
			} else {
				goto end;
			}
		}
		if ((buf_len - (p - buf)) > 0) {
			error_msg_len = MIN((int)((buf_len - (p - buf))), (int) (error_buf_len - 1));
			memcpy(error, p, error_msg_len);
		}
	}
end:
	sqlstate[MYSQLND_SQLSTATE_LENGTH] = '\0';
	error[error_msg_len]= '\0';

	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ mysqlnd_read_header */
static enum_func_status
mysqlnd_read_header(MYSQLND * conn, MYSQLND_PACKET_HEADER * header TSRMLS_DC)
{
	MYSQLND_NET * net = conn->net;
	zend_uchar buffer[MYSQLND_HEADER_SIZE];

	DBG_ENTER("mysqlnd_read_header_name");
	DBG_INF_FMT("compressed=%u conn_id=%u", net->compressed, conn->thread_id);
	if (FAIL == net->m.receive(conn, buffer, MYSQLND_HEADER_SIZE TSRMLS_CC)) {
		DBG_RETURN(FAIL);
	}

	header->size = uint3korr(buffer);
	header->packet_no = uint1korr(buffer + 3);

#ifdef MYSQLND_DUMP_HEADER_N_BODY
	DBG_INF_FMT("HEADER: prot_packet_no=%u size=%3u", header->packet_no, header->size);
#endif
	MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn->stats,
							STAT_PROTOCOL_OVERHEAD_IN, MYSQLND_HEADER_SIZE,
							STAT_PACKETS_RECEIVED, 1);

	if (net->compressed || net->packet_no == header->packet_no) {
		/*
		  Have to increase the number, so we can send correct number back. It will
		  round at 255 as this is unsigned char. The server needs this for simple
		  flow control checking.
		*/
		net->packet_no++;
		DBG_RETURN(PASS);
	}

	DBG_ERR_FMT("Logical link: packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
				net->packet_no, header->packet_no, header->size);

	php_error(E_WARNING, "Packets out of order. Expected %u received %u. Packet size="MYSQLND_SZ_T_SPEC,
			  net->packet_no, header->packet_no, header->size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_greet_read */
static enum_func_status
php_mysqlnd_greet_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	zend_uchar buf[2048];
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	MYSQLND_PACKET_GREET *packet= (MYSQLND_PACKET_GREET *) _packet;

	DBG_ENTER("php_mysqlnd_greet_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, sizeof(buf), "greeting", PROT_GREET_PACKET);
	BAIL_IF_NO_MORE_DATA;

	packet->protocol_version = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->protocol_version) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										 TSRMLS_CC);
		/*
		  The server doesn't send sqlstate in the greet packet.
		  It's a bug#26426 , so we have to set it correctly ourselves.
		  It's probably "Too many connections, which has SQL state 08004".
		*/
		if (packet->error_no == 1040) {
			memcpy(packet->sqlstate, "08004", MYSQLND_SQLSTATE_LENGTH);
		}
		DBG_RETURN(PASS);
	}

	packet->server_version = estrdup((char *)p);
	p+= strlen(packet->server_version) + 1; /* eat the '\0' */
	BAIL_IF_NO_MORE_DATA;

	packet->thread_id = uint4korr(p);
	p+=4;
	BAIL_IF_NO_MORE_DATA;

	memcpy(packet->scramble_buf, p, SCRAMBLE_LENGTH_323);
	p+= 8;
	BAIL_IF_NO_MORE_DATA;

	/* pad1 */
	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->server_capabilities = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	packet->charset_no = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	packet->server_status = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	/* pad2 */
	p+= 13;
	BAIL_IF_NO_MORE_DATA;

	if ((size_t) (p - buf) < packet->header.size) {
		/* scramble_buf is split into two parts */
		memcpy(packet->scramble_buf + SCRAMBLE_LENGTH_323,
				p, SCRAMBLE_LENGTH - SCRAMBLE_LENGTH_323);
	} else {
		packet->pre41 = TRUE;
	}

	DBG_INF_FMT("proto=%u server=%s thread_id=%u",
				packet->protocol_version, packet->server_version, packet->thread_id);

	DBG_INF_FMT("server_capabilities=%u charset_no=%u server_status=%i",
				packet->server_capabilities, packet->charset_no, packet->server_status);

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("GREET packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "GREET packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_greet_free_mem */
static
void php_mysqlnd_greet_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_GREET *p= (MYSQLND_PACKET_GREET *) _packet;
	if (p->server_version) {
		efree(p->server_version);
		p->server_version = NULL;
	}
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_crypt */
static void
php_mysqlnd_crypt(zend_uchar *buffer, const zend_uchar *s1, const zend_uchar *s2, size_t len)
{
	const zend_uchar *s1_end = s1 + len;
	while (s1 < s1_end) {
		*buffer++= *s1++ ^ *s2++;
	}
}
/* }}} */


/* {{{ php_mysqlnd_scramble */
void php_mysqlnd_scramble(zend_uchar * const buffer, const zend_uchar * const scramble, const zend_uchar * const password)
{
	PHP_SHA1_CTX context;
	zend_uchar sha1[SHA1_MAX_LENGTH];
	zend_uchar sha2[SHA1_MAX_LENGTH];


	/* Phase 1: hash password */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, password, strlen((char *)password));
	PHP_SHA1Final(sha1, &context);

	/* Phase 2: hash sha1 */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, (zend_uchar*)sha1, SHA1_MAX_LENGTH);
	PHP_SHA1Final(sha2, &context);

	/* Phase 3: hash scramble + sha2 */
	PHP_SHA1Init(&context);
	PHP_SHA1Update(&context, scramble, SCRAMBLE_LENGTH);
	PHP_SHA1Update(&context, (zend_uchar*)sha2, SHA1_MAX_LENGTH);
	PHP_SHA1Final(buffer, &context);

	/* let's crypt buffer now */
	php_mysqlnd_crypt(buffer, (const zend_uchar *)buffer, (const zend_uchar *)sha1, SHA1_MAX_LENGTH);
}
/* }}} */


#define AUTH_WRITE_BUFFER_LEN (MYSQLND_HEADER_SIZE + MYSQLND_MAX_ALLOWED_USER_LEN + SHA1_MAX_LENGTH + MYSQLND_MAX_ALLOWED_DB_LEN + 1 + 128)

/* {{{ php_mysqlnd_auth_write */
static
size_t php_mysqlnd_auth_write(void *_packet, MYSQLND * conn TSRMLS_DC)
{
	char buffer[AUTH_WRITE_BUFFER_LEN];
	register char *p= buffer + MYSQLND_HEADER_SIZE; /* start after the header */
	int len;
	register MYSQLND_PACKET_AUTH *packet= (MYSQLND_PACKET_AUTH *) _packet;

	DBG_ENTER("php_mysqlnd_auth_write");

	int4store(p, packet->client_flags);
	p+= 4;

	int4store(p, packet->max_packet_size);
	p+= 4;

	int1store(p, packet->charset_no);
	p++;

	memset(p, 0, 23); /* filler */
	p+= 23;

	if (!packet->send_half_packet) {
		len = MIN(strlen(packet->user), MYSQLND_MAX_ALLOWED_USER_LEN);
		memcpy(p, packet->user, len);
		p+= len;
		*p++ = '\0';

		/* copy scrambled pass*/
		if (packet->password && packet->password[0]) {
			/* In 4.1 we use CLIENT_SECURE_CONNECTION and thus the len of the buf should be passed */
			int1store(p, SHA1_MAX_LENGTH);
			p++;
			php_mysqlnd_scramble((zend_uchar*)p, packet->server_scramble_buf, (zend_uchar*)packet->password);
			p+= SHA1_MAX_LENGTH;
		} else {
			/* Zero length */
			int1store(p, 0);
			p++;
		}

		if (packet->db) {
			size_t real_db_len = MIN(MYSQLND_MAX_ALLOWED_DB_LEN, packet->db_len);
			memcpy(p, packet->db, real_db_len);
			p+= real_db_len;
			*p++= '\0';
		}
		/* Handle CLIENT_CONNECT_WITH_DB */
		/* no \0 for no DB */
	}

	DBG_RETURN(conn->net->m.send(conn, buffer, p - buffer - MYSQLND_HEADER_SIZE TSRMLS_CC));
}
/* }}} */


/* {{{ php_mysqlnd_auth_free_mem */
static
void php_mysqlnd_auth_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	if (!stack_allocation) {
		MYSQLND_PACKET_AUTH * p = (MYSQLND_PACKET_AUTH *) _packet;
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


#define OK_BUFFER_SIZE 2048

/* {{{ php_mysqlnd_ok_read */
static enum_func_status
php_mysqlnd_ok_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	zend_uchar local_buf[OK_BUFFER_SIZE];
	size_t buf_len = conn->net->cmd_buffer.buffer? conn->net->cmd_buffer.length : OK_BUFFER_SIZE;
	zend_uchar *buf = conn->net->cmd_buffer.buffer? (zend_uchar *) conn->net->cmd_buffer.buffer : local_buf;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	unsigned long i;
	register MYSQLND_PACKET_OK *packet= (MYSQLND_PACKET_OK *) _packet;

	DBG_ENTER("php_mysqlnd_ok_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "OK", PROT_OK_PACKET);
	BAIL_IF_NO_MORE_DATA;

	/* Should be always 0x0 or ERROR_MARKER for error */
	packet->field_count = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->field_count) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										 TSRMLS_CC);
		DBG_RETURN(PASS);
	}
	/* Everything was fine! */
	packet->affected_rows  = php_mysqlnd_net_field_length_ll(&p);
	BAIL_IF_NO_MORE_DATA;

	packet->last_insert_id = php_mysqlnd_net_field_length_ll(&p);
	BAIL_IF_NO_MORE_DATA;

	packet->server_status = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	packet->warning_count = uint2korr(p);
	p+= 2;
	BAIL_IF_NO_MORE_DATA;

	/* There is a message */
	if (packet->header.size > (size_t) (p - buf) && (i = php_mysqlnd_net_field_length(&p))) {
		packet->message_len = MIN(i, buf_len - (p - begin));
		packet->message = mnd_pestrndup((char *)p, packet->message_len, FALSE);
	} else {
		packet->message = NULL;
		packet->message_len = 0;
	}

	DBG_INF_FMT("OK packet: aff_rows=%lld last_ins_id=%ld server_status=%u warnings=%u",
				packet->affected_rows, packet->last_insert_id, packet->server_status,
				packet->warning_count);

	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("OK packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "OK packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_ok_free_mem */
static void
php_mysqlnd_ok_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_OK *p= (MYSQLND_PACKET_OK *) _packet;
	if (p->message) {
		mnd_efree(p->message);
		p->message = NULL;
	}
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_eof_read */
static enum_func_status
php_mysqlnd_eof_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/*
	  EOF packet is since 4.1 five bytes long,
	  but we can get also an error, make it bigger.

	  Error : error_code + '#' + sqlstate + MYSQLND_ERRMSG_SIZE
	*/
	MYSQLND_PACKET_EOF *packet= (MYSQLND_PACKET_EOF *) _packet;
	size_t buf_len = conn->net->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;

	DBG_ENTER("php_mysqlnd_eof_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "EOF", PROT_EOF_PACKET);
	BAIL_IF_NO_MORE_DATA;

	/* Should be always EODATA_MARKER */
	packet->field_count = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->field_count) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error, sizeof(packet->error),
										 &packet->error_no, packet->sqlstate
										 TSRMLS_CC);
		DBG_RETURN(PASS);
	}

	/*
		4.1 sends 1 byte EOF packet after metadata of
		PREPARE/EXECUTE but 5 bytes after the result. This is not
		according to the Docs@Forge!!!
	*/
	if (packet->header.size > 1) {
		packet->warning_count = uint2korr(p);
		p+= 2;
		BAIL_IF_NO_MORE_DATA;

		packet->server_status = uint2korr(p);
		p+= 2;
		BAIL_IF_NO_MORE_DATA;
	} else {
		packet->warning_count = 0;
		packet->server_status = 0;
	}

	BAIL_IF_NO_MORE_DATA;

	DBG_INF_FMT("EOF packet: fields=%u status=%u warnings=%u",
				packet->field_count, packet->server_status, packet->warning_count);

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("EOF packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "EOF packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_eof_free_mem */
static
void php_mysqlnd_eof_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	if (!stack_allocation) {
		mnd_pefree(_packet, ((MYSQLND_PACKET_EOF *)_packet)->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_cmd_write */
size_t php_mysqlnd_cmd_write(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/* Let's have some space, which we can use, if not enough, we will allocate new buffer */
	MYSQLND_PACKET_COMMAND *packet= (MYSQLND_PACKET_COMMAND *) _packet;
	MYSQLND_NET *net = conn->net;
	unsigned int error_reporting = EG(error_reporting);
	size_t written = 0;

	DBG_ENTER("php_mysqlnd_cmd_write");
	/*
	  Reset packet_no, or we will get bad handshake!
	  Every command starts a new TX and packet numbers are reset to 0.
	*/
	net->packet_no = 0;
	net->compressed_envelope_packet_no = 0; /* this is for the response */

	if (error_reporting) {
		EG(error_reporting) = 0;
	}

	MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_PACKETS_SENT_CMD);

#ifdef MYSQLND_DO_WIRE_CHECK_BEFORE_COMMAND
	net->m.consume_uneaten_data(net, packet->command TSRMLS_CC);
#endif

	if (!packet->argument || !packet->arg_len) {
		char buffer[MYSQLND_HEADER_SIZE + 1];

		int1store(buffer + MYSQLND_HEADER_SIZE, packet->command);
		written = conn->net->m.send(conn, buffer, 1 TSRMLS_CC);
	} else {
		size_t tmp_len = packet->arg_len + 1 + MYSQLND_HEADER_SIZE, ret;
		zend_uchar *tmp, *p;
		tmp = (tmp_len > net->cmd_buffer.length)? mnd_emalloc(tmp_len):net->cmd_buffer.buffer;
		if (!tmp) {
			goto end;
		}
		p = tmp + MYSQLND_HEADER_SIZE; /* skip the header */

		int1store(p, packet->command);
		p++;

		memcpy(p, packet->argument, packet->arg_len);

		ret = conn->net->m.send(conn, (char *)tmp, tmp_len - MYSQLND_HEADER_SIZE TSRMLS_CC);
		if (tmp != net->cmd_buffer.buffer) {
			MYSQLND_INC_CONN_STATISTIC(conn->stats, STAT_CMD_BUFFER_TOO_SMALL);
			mnd_efree(tmp);
		}
		written = ret;
	}
end:
	if (error_reporting) {
		/* restore error reporting */
		EG(error_reporting) = error_reporting;
	}
	DBG_RETURN(written);
}
/* }}} */


/* {{{ php_mysqlnd_cmd_free_mem */
static
void php_mysqlnd_cmd_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	if (!stack_allocation) {
		MYSQLND_PACKET_COMMAND * p = (MYSQLND_PACKET_COMMAND *) _packet;
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_rset_header_read */
static enum_func_status
php_mysqlnd_rset_header_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	enum_func_status ret = PASS;
	size_t buf_len = conn->net->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	size_t len;
	MYSQLND_PACKET_RSET_HEADER *packet= (MYSQLND_PACKET_RSET_HEADER *) _packet;

	DBG_ENTER("php_mysqlnd_rset_header_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "resultset header", PROT_RSET_HEADER_PACKET);
	BAIL_IF_NO_MORE_DATA;

	/*
	  Don't increment. First byte is ERROR_MARKER on error, but otherwise is starting byte
	  of encoded sequence for length.
	*/
	if (ERROR_MARKER == *p) {
		/* Error */
		p++;
		BAIL_IF_NO_MORE_DATA;
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error, sizeof(packet->error_info.error),
										 &packet->error_info.error_no, packet->error_info.sqlstate
										 TSRMLS_CC);
		DBG_RETURN(PASS);
	}

	packet->field_count = php_mysqlnd_net_field_length(&p);
	BAIL_IF_NO_MORE_DATA;

	switch (packet->field_count) {
		case MYSQLND_NULL_LENGTH:
			DBG_INF("LOAD LOCAL");
			/*
			  First byte in the packet is the field count.
			  Thus, the name is size - 1. And we add 1 for a trailing \0.
			  Because we have BAIL_IF_NO_MORE_DATA before the switch, we are guaranteed
			  that packet->header.size is > 0. Which means that len can't underflow, that
			  would lead to 0 byte allocation but 2^32 or 2^64 bytes copied.
			*/
			len = packet->header.size - 1;
			packet->info_or_local_file = mnd_emalloc(len + 1);
			if (packet->info_or_local_file) {
				memcpy(packet->info_or_local_file, p, len);
				packet->info_or_local_file[len] = '\0';
				packet->info_or_local_file_len = len;
			} else {
				SET_OOM_ERROR(conn->error_info);
				ret = FAIL;
			}
			break;
		case 0x00:
			DBG_INF("UPSERT");
			packet->affected_rows = php_mysqlnd_net_field_length_ll(&p);
			BAIL_IF_NO_MORE_DATA;

			packet->last_insert_id = php_mysqlnd_net_field_length_ll(&p);
			BAIL_IF_NO_MORE_DATA;

			packet->server_status = uint2korr(p);
			p+=2;
			BAIL_IF_NO_MORE_DATA;

			packet->warning_count = uint2korr(p);
			p+=2;
			BAIL_IF_NO_MORE_DATA;
			/* Check for additional textual data */
			if (packet->header.size  > (size_t) (p - buf) && (len = php_mysqlnd_net_field_length(&p))) {
				packet->info_or_local_file = mnd_emalloc(len + 1);
				if (packet->info_or_local_file) {
					memcpy(packet->info_or_local_file, p, len);
					packet->info_or_local_file[len] = '\0';
					packet->info_or_local_file_len = len;
				} else {
					SET_OOM_ERROR(conn->error_info);
					ret = FAIL;
				}
			}
			DBG_INF_FMT("affected_rows=%llu last_insert_id=%llu server_status=%u warning_count=%u",
						packet->affected_rows, packet->last_insert_id,
						packet->server_status, packet->warning_count);
			break;
		default:
			DBG_INF("SELECT");
			/* Result set */
			break;
	}
	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(ret);
premature_end:
	DBG_ERR_FMT("RSET_HEADER packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "RSET_HEADER packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_rset_header_free_mem */
static
void php_mysqlnd_rset_header_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_RSET_HEADER *p= (MYSQLND_PACKET_RSET_HEADER *) _packet;
	DBG_ENTER("php_mysqlnd_rset_header_free_mem");
	if (p->info_or_local_file) {
		mnd_efree(p->info_or_local_file);
		p->info_or_local_file = NULL;
	}
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
	DBG_VOID_RETURN;
}
/* }}} */

static size_t rset_field_offsets[] =
{
	STRUCT_OFFSET(MYSQLND_FIELD, catalog),
	STRUCT_OFFSET(MYSQLND_FIELD, catalog_length),
	STRUCT_OFFSET(MYSQLND_FIELD, db),
	STRUCT_OFFSET(MYSQLND_FIELD, db_length),
	STRUCT_OFFSET(MYSQLND_FIELD, table),
	STRUCT_OFFSET(MYSQLND_FIELD, table_length),
	STRUCT_OFFSET(MYSQLND_FIELD, org_table),
	STRUCT_OFFSET(MYSQLND_FIELD, org_table_length),
	STRUCT_OFFSET(MYSQLND_FIELD, name),
	STRUCT_OFFSET(MYSQLND_FIELD, name_length),
	STRUCT_OFFSET(MYSQLND_FIELD, org_name),
	STRUCT_OFFSET(MYSQLND_FIELD, org_name_length)
};


/* {{{ php_mysqlnd_rset_field_read */
static enum_func_status
php_mysqlnd_rset_field_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/* Should be enough for the metadata of a single row */
	MYSQLND_PACKET_RES_FIELD *packet= (MYSQLND_PACKET_RES_FIELD *) _packet;
	size_t buf_len = conn->net->cmd_buffer.length, total_len = 0;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	char *root_ptr;
	unsigned long len;
	MYSQLND_FIELD *meta;
	unsigned int i, field_count = sizeof(rset_field_offsets)/sizeof(size_t);

	DBG_ENTER("php_mysqlnd_rset_field_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "field", PROT_RSET_FLD_PACKET);

	if (packet->skip_parsing) {
		DBG_RETURN(PASS);
	}

	BAIL_IF_NO_MORE_DATA;
	if (ERROR_MARKER == *p) {
		/* Error */
		p++;
		BAIL_IF_NO_MORE_DATA;
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error, sizeof(packet->error_info.error),
										 &packet->error_info.error_no, packet->error_info.sqlstate
										 TSRMLS_CC);
		DBG_ERR_FMT("Server error : (%u) %s", packet->error_info.error_no, packet->error_info.error);
		DBG_RETURN(PASS);
	} else if (EODATA_MARKER == *p && packet->header.size < 8) {
		/* Premature EOF. That should be COM_FIELD_LIST */
		DBG_INF("Premature EOF. That should be COM_FIELD_LIST");
		packet->stupid_list_fields_eof = TRUE;
		DBG_RETURN(PASS);
	}

	meta = packet->metadata;

	for (i = 0; i < field_count; i += 2) {
		len = php_mysqlnd_net_field_length(&p);
		BAIL_IF_NO_MORE_DATA;
		switch ((len)) {
			case 0:
				*(const char **)(((char*)meta) + rset_field_offsets[i]) = mysqlnd_empty_string;
				*(unsigned int *)(((char*)meta) + rset_field_offsets[i+1]) = 0;
				break;
			case MYSQLND_NULL_LENGTH:
				goto faulty_or_fake;
			default:
				*(const char **)(((char *)meta) + rset_field_offsets[i]) = (const char *)p;
				*(unsigned int *)(((char*)meta) + rset_field_offsets[i+1]) = len;
				p += len;
				total_len += len + 1;
				break;
		}
		BAIL_IF_NO_MORE_DATA;
	}

	/* 1 byte length */
	if (12 != *p) {
		DBG_ERR_FMT("Protocol error. Server sent false length. Expected 12 got %d", (int) *p);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Protocol error. Server sent false length. Expected 12");
	}
	p++;
	BAIL_IF_NO_MORE_DATA;

	meta->charsetnr = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	meta->length = uint4korr(p);
	p += 4;
	BAIL_IF_NO_MORE_DATA;

	meta->type = uint1korr(p);
	p += 1;
	BAIL_IF_NO_MORE_DATA;

	meta->flags = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	meta->decimals = uint2korr(p);
	p += 1;
	BAIL_IF_NO_MORE_DATA;

	/* 2 byte filler */
	p +=2;
	BAIL_IF_NO_MORE_DATA;

	/* Should we set NUM_FLAG (libmysql does it) ? */
	if (
		(meta->type <= MYSQL_TYPE_INT24 &&
			(meta->type != MYSQL_TYPE_TIMESTAMP || meta->length == 14 || meta->length == 8)
		) || meta->type == MYSQL_TYPE_YEAR)
	{
		meta->flags |= NUM_FLAG;
	}


	/*
	  def could be empty, thus don't allocate on the root.
	  NULL_LENGTH (0xFB) comes from COM_FIELD_LIST when the default value is NULL.
	  Otherwise the string is length encoded.
	*/
	if (packet->header.size > (size_t) (p - buf) &&
		(len = php_mysqlnd_net_field_length(&p)) &&
		len != MYSQLND_NULL_LENGTH)
	{
		BAIL_IF_NO_MORE_DATA;
		DBG_INF_FMT("Def found, length %lu, persistent=%u", len, packet->persistent_alloc);
		meta->def = mnd_pemalloc(len + 1, packet->persistent_alloc);
		if (!meta->def) {
			SET_OOM_ERROR(conn->error_info);
			DBG_RETURN(FAIL);
		}
		memcpy(meta->def, p, len);
		meta->def[len] = '\0';
		meta->def_length = len;
		p += len;
	}

	DBG_INF_FMT("allocing root. persistent=%u", packet->persistent_alloc);
	root_ptr = meta->root = mnd_pemalloc(total_len, packet->persistent_alloc);
	if (!root_ptr) {
		SET_OOM_ERROR(conn->error_info);
		DBG_RETURN(FAIL);
	}

	meta->root_len = total_len;
	/* Now do allocs */
	if (meta->catalog && meta->catalog != mysqlnd_empty_string) {
		len = meta->catalog_length;
		meta->catalog = memcpy(root_ptr, meta->catalog, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->db && meta->db != mysqlnd_empty_string) {
		len = meta->db_length;
		meta->db = memcpy(root_ptr, meta->db, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->table && meta->table != mysqlnd_empty_string) {
		len = meta->table_length;
		meta->table = memcpy(root_ptr, meta->table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_table && meta->org_table != mysqlnd_empty_string) {
		len = meta->org_table_length;
		meta->org_table = memcpy(root_ptr, meta->org_table, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->name && meta->name != mysqlnd_empty_string) {
		len = meta->name_length;
		meta->name = memcpy(root_ptr, meta->name, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	if (meta->org_name && meta->org_name != mysqlnd_empty_string) {
		len = meta->org_name_length;
		meta->org_name = memcpy(root_ptr, meta->org_name, len);
		*(root_ptr +=len) = '\0';
		root_ptr++;
	}

	DBG_INF_FMT("FIELD=[%s.%s.%s]", meta->db? meta->db:"*NA*", meta->table? meta->table:"*NA*",
				meta->name? meta->name:"*NA*");

	DBG_RETURN(PASS);

faulty_or_fake:
	DBG_ERR_FMT("Protocol error. Server sent NULL_LENGTH. The server is faulty");
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Protocol error. Server sent NULL_LENGTH."
					 " The server is faulty");
	DBG_RETURN(FAIL);
premature_end:
	DBG_ERR_FMT("RSET field packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "Result set field packet "MYSQLND_SZ_T_SPEC" bytes "
			 		"shorter than expected", p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_rset_field_free_mem */
static
void php_mysqlnd_rset_field_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_RES_FIELD *p= (MYSQLND_PACKET_RES_FIELD *) _packet;
	/* p->metadata was passed to us as temporal buffer */
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_read_row_ex */
static enum_func_status
php_mysqlnd_read_row_ex(MYSQLND * conn, MYSQLND_MEMORY_POOL * result_set_memory_pool,
						MYSQLND_MEMORY_POOL_CHUNK **buffer,
						size_t *data_size, zend_bool persistent_alloc,
						unsigned int prealloc_more_bytes TSRMLS_DC)
{
	enum_func_status ret = PASS;
	MYSQLND_PACKET_HEADER header;
	zend_uchar *p = NULL;
	zend_bool first_iteration = TRUE;

	DBG_ENTER("php_mysqlnd_read_row_ex");

	/*
	  To ease the process the server splits everything in packets up to 2^24 - 1.
	  Even in the case the payload is evenly divisible by this value, the last
	  packet will be empty, namely 0 bytes. Thus, we can read every packet and ask
	  for next one if they have 2^24 - 1 sizes. But just read the header of a
	  zero-length byte, don't read the body, there is no such.
	*/

	*data_size = prealloc_more_bytes;
	while (1) {
		if (FAIL == mysqlnd_read_header(conn , &header TSRMLS_CC)) {
			ret = FAIL;
			break;
		}

		*data_size += header.size;

		if (first_iteration) {
			first_iteration = FALSE;
			/*
			  We need a trailing \0 for the last string, in case of text-mode,
			  to be able to implement read-only variables. Thus, we add + 1.
			*/
			*buffer = result_set_memory_pool->get_chunk(result_set_memory_pool, *data_size + 1 TSRMLS_CC);
			if (!*buffer) {
				ret = FAIL;
				break;
			}
			p = (*buffer)->ptr;
		} else if (!first_iteration) {
			/* Empty packet after MYSQLND_MAX_PACKET_SIZE packet. That's ok, break */
			if (!header.size) {
				break;
			}

			/*
			  We have to realloc the buffer.

			  We need a trailing \0 for the last string, in case of text-mode,
			  to be able to implement read-only variables.
			*/
			if (FAIL == (*buffer)->resize_chunk((*buffer), *data_size + 1 TSRMLS_CC)) {
				SET_OOM_ERROR(conn->error_info);
				ret = FAIL;
				break;
			}
			/* The position could have changed, recalculate */
			p = (*buffer)->ptr + (*data_size - header.size);
		}

		if (PASS != (ret = conn->net->m.receive(conn, p, header.size TSRMLS_CC))) {
			DBG_ERR("Empty row packet body");
			php_error(E_WARNING, "Empty row packet body");
			break;
		}

		if (header.size < MYSQLND_MAX_PACKET_SIZE) {
			break;
		}
	}
	if (ret == FAIL && *buffer) {
		(*buffer)->free_chunk((*buffer) TSRMLS_CC);
		*buffer = NULL;
	}
	*data_size -= prealloc_more_bytes;
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read_binary_protocol */
enum_func_status
php_mysqlnd_rowp_read_binary_protocol(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
									  unsigned int field_count, MYSQLND_FIELD *fields_metadata,
									  zend_bool persistent,
									  zend_bool as_unicode, zend_bool as_int_or_float,
									  MYSQLND_STATS * stats TSRMLS_DC)
{
	unsigned int i;
	zend_uchar *p = row_buffer->ptr;
	zend_uchar *null_ptr, bit;
	zval **current_field, **end_field, **start_field;

	DBG_ENTER("php_mysqlnd_rowp_read_binary_protocol");

	if (!fields) {
		DBG_RETURN(FAIL);
	}

	end_field = (start_field = fields) + field_count;

	/* skip the first byte, not EODATA_MARKER -> 0x0, status */
	p++;
	null_ptr= p;
	p += (field_count + 9)/8;	/* skip null bits */
	bit	= 4;					/* first 2 bits are reserved */

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
		DBG_INF("Directly creating zval");
		MAKE_STD_ZVAL(*current_field);
		if (!*current_field) {
			DBG_RETURN(FAIL);
		}
	}

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
		enum_mysqlnd_collected_stats statistic;
		zend_uchar * orig_p = p;

		DBG_INF_FMT("Into zval=%p decoding column %u [%s.%s.%s] type=%u field->flags&unsigned=%u flags=%u is_bit=%u as_unicode=%u",
			*current_field, i,
			fields_metadata[i].db, fields_metadata[i].table, fields_metadata[i].name, fields_metadata[i].type,
			fields_metadata[i].flags & UNSIGNED_FLAG, fields_metadata[i].flags, fields_metadata[i].type == MYSQL_TYPE_BIT, as_unicode);
		if (*null_ptr & bit) {
			DBG_INF("It's null");
			ZVAL_NULL(*current_field);
			statistic = STAT_BINARY_TYPE_FETCHED_NULL;
		} else {
			enum_mysqlnd_field_types type = fields_metadata[i].type;
			mysqlnd_ps_fetch_functions[type].func(*current_field, &fields_metadata[i], 0, &p, as_unicode TSRMLS_CC);

			if (MYSQLND_G(collect_statistics)) {
				switch (fields_metadata[i].type) {
					case MYSQL_TYPE_DECIMAL:	statistic = STAT_BINARY_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_TINY:		statistic = STAT_BINARY_TYPE_FETCHED_INT8; break;
					case MYSQL_TYPE_SHORT:		statistic = STAT_BINARY_TYPE_FETCHED_INT16; break;
					case MYSQL_TYPE_LONG:		statistic = STAT_BINARY_TYPE_FETCHED_INT32; break;
					case MYSQL_TYPE_FLOAT:		statistic = STAT_BINARY_TYPE_FETCHED_FLOAT; break;
					case MYSQL_TYPE_DOUBLE:		statistic = STAT_BINARY_TYPE_FETCHED_DOUBLE; break;
					case MYSQL_TYPE_NULL:		statistic = STAT_BINARY_TYPE_FETCHED_NULL; break;
					case MYSQL_TYPE_TIMESTAMP:	statistic = STAT_BINARY_TYPE_FETCHED_TIMESTAMP; break;
					case MYSQL_TYPE_LONGLONG:	statistic = STAT_BINARY_TYPE_FETCHED_INT64; break;
					case MYSQL_TYPE_INT24:		statistic = STAT_BINARY_TYPE_FETCHED_INT24; break;
					case MYSQL_TYPE_DATE:		statistic = STAT_BINARY_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_TIME:		statistic = STAT_BINARY_TYPE_FETCHED_TIME; break;
					case MYSQL_TYPE_DATETIME:	statistic = STAT_BINARY_TYPE_FETCHED_DATETIME; break;
					case MYSQL_TYPE_YEAR:		statistic = STAT_BINARY_TYPE_FETCHED_YEAR; break;
					case MYSQL_TYPE_NEWDATE:	statistic = STAT_BINARY_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_VARCHAR:	statistic = STAT_BINARY_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_BIT:		statistic = STAT_BINARY_TYPE_FETCHED_BIT; break;
					case MYSQL_TYPE_NEWDECIMAL:	statistic = STAT_BINARY_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_ENUM:		statistic = STAT_BINARY_TYPE_FETCHED_ENUM; break;
					case MYSQL_TYPE_SET:		statistic = STAT_BINARY_TYPE_FETCHED_SET; break;
					case MYSQL_TYPE_TINY_BLOB:	statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_MEDIUM_BLOB:statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_LONG_BLOB:	statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_BLOB:		statistic = STAT_BINARY_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_VAR_STRING:	statistic = STAT_BINARY_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_STRING:		statistic = STAT_BINARY_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_GEOMETRY:	statistic = STAT_BINARY_TYPE_FETCHED_GEOMETRY; break;
					default: statistic = STAT_BINARY_TYPE_FETCHED_OTHER; break;
				}
			}
		}
		MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, statistic, 1,
										STAT_BYTES_RECEIVED_PURE_DATA_PS,
										(Z_TYPE_PP(current_field) == IS_STRING)?
											Z_STRLEN_PP(current_field) : (p - orig_p));

		if (!((bit<<=1) & 255)) {
			bit = 1;	/* to the following byte */
			null_ptr++;
		}
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read_text_protocol */
enum_func_status
php_mysqlnd_rowp_read_text_protocol(MYSQLND_MEMORY_POOL_CHUNK * row_buffer, zval ** fields,
									unsigned int field_count, MYSQLND_FIELD *fields_metadata,
									zend_bool persistent,
									zend_bool as_unicode, zend_bool as_int_or_float,
									MYSQLND_STATS * stats TSRMLS_DC)
{
	unsigned int i;
	zend_bool last_field_was_string = FALSE;
	zval **current_field, **end_field, **start_field;
	zend_uchar *p = row_buffer->ptr;
	size_t data_size = row_buffer->app;
	zend_uchar *bit_area = (zend_uchar*) row_buffer->ptr + data_size + 1; /* we allocate from here */

	DBG_ENTER("php_mysqlnd_rowp_read_text_protocol");

	if (!fields) {
		DBG_RETURN(FAIL);
	}

	end_field = (start_field = fields) + field_count;

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
		DBG_INF("Directly creating zval");
		MAKE_STD_ZVAL(*current_field);
		if (!*current_field) {
			DBG_RETURN(FAIL);
		}
	}

	for (i = 0, current_field = start_field; current_field < end_field; current_field++, i++) {
		/* Don't reverse the order. It is significant!*/
		zend_uchar *this_field_len_pos = p;
		/* php_mysqlnd_net_field_length() call should be after *this_field_len_pos = p; */
		unsigned long len = php_mysqlnd_net_field_length(&p);

		if (current_field > start_field && last_field_was_string) {
			/*
			  Normal queries:
			  We have to put \0 now to the end of the previous field, if it was
			  a string. IS_NULL doesn't matter. Because we have already read our
			  length, then we can overwrite it in the row buffer.
			  This statement terminates the previous field, not the current one.

			  NULL_LENGTH is encoded in one byte, so we can stick a \0 there.
			  Any string's length is encoded in at least one byte, so we can stick
			  a \0 there.
			*/

			*this_field_len_pos = '\0';
		}

		/* NULL or NOT NULL, this is the question! */
		if (len == MYSQLND_NULL_LENGTH) {
			ZVAL_NULL(*current_field);
			last_field_was_string = FALSE;
		} else {
#if MYSQLND_UNICODE || defined(MYSQLND_STRING_TO_INT_CONVERSION)
			struct st_mysqlnd_perm_bind perm_bind =
					mysqlnd_ps_fetch_functions[fields_metadata[i].type];
#endif
			if (MYSQLND_G(collect_statistics)) {
				enum_mysqlnd_collected_stats statistic;
				switch (fields_metadata[i].type) {
					case MYSQL_TYPE_DECIMAL:	statistic = STAT_TEXT_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_TINY:		statistic = STAT_TEXT_TYPE_FETCHED_INT8; break;
					case MYSQL_TYPE_SHORT:		statistic = STAT_TEXT_TYPE_FETCHED_INT16; break;
					case MYSQL_TYPE_LONG:		statistic = STAT_TEXT_TYPE_FETCHED_INT32; break;
					case MYSQL_TYPE_FLOAT:		statistic = STAT_TEXT_TYPE_FETCHED_FLOAT; break;
					case MYSQL_TYPE_DOUBLE:		statistic = STAT_TEXT_TYPE_FETCHED_DOUBLE; break;
					case MYSQL_TYPE_NULL:		statistic = STAT_TEXT_TYPE_FETCHED_NULL; break;
					case MYSQL_TYPE_TIMESTAMP:	statistic = STAT_TEXT_TYPE_FETCHED_TIMESTAMP; break;
					case MYSQL_TYPE_LONGLONG:	statistic = STAT_TEXT_TYPE_FETCHED_INT64; break;
					case MYSQL_TYPE_INT24:		statistic = STAT_TEXT_TYPE_FETCHED_INT24; break;
					case MYSQL_TYPE_DATE:		statistic = STAT_TEXT_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_TIME:		statistic = STAT_TEXT_TYPE_FETCHED_TIME; break;
					case MYSQL_TYPE_DATETIME:	statistic = STAT_TEXT_TYPE_FETCHED_DATETIME; break;
					case MYSQL_TYPE_YEAR:		statistic = STAT_TEXT_TYPE_FETCHED_YEAR; break;
					case MYSQL_TYPE_NEWDATE:	statistic = STAT_TEXT_TYPE_FETCHED_DATE; break;
					case MYSQL_TYPE_VARCHAR:	statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_BIT:		statistic = STAT_TEXT_TYPE_FETCHED_BIT; break;
					case MYSQL_TYPE_NEWDECIMAL:	statistic = STAT_TEXT_TYPE_FETCHED_DECIMAL; break;
					case MYSQL_TYPE_ENUM:		statistic = STAT_TEXT_TYPE_FETCHED_ENUM; break;
					case MYSQL_TYPE_SET:		statistic = STAT_TEXT_TYPE_FETCHED_SET; break;
					case MYSQL_TYPE_TINY_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_MEDIUM_BLOB:statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_LONG_BLOB:	statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_BLOB:		statistic = STAT_TEXT_TYPE_FETCHED_BLOB; break;
					case MYSQL_TYPE_VAR_STRING:	statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_STRING:		statistic = STAT_TEXT_TYPE_FETCHED_STRING; break;
					case MYSQL_TYPE_GEOMETRY:	statistic = STAT_TEXT_TYPE_FETCHED_GEOMETRY; break;
					default: statistic = STAT_TEXT_TYPE_FETCHED_OTHER; break;
				}
				MYSQLND_INC_CONN_STATISTIC_W_VALUE2(stats, statistic, 1, STAT_BYTES_RECEIVED_PURE_DATA_TEXT, len);
			}
#ifdef MYSQLND_STRING_TO_INT_CONVERSION
			if (as_int_or_float && perm_bind.php_type == IS_LONG) {
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				if (perm_bind.pack_len < SIZEOF_LONG) {
					/* direct conversion */
					int64_t v =
#ifndef PHP_WIN32
						atoll((char *) p);
#else
						_atoi64((char *) p);
#endif
					ZVAL_LONG(*current_field, (long) v); /* the cast is safe */
				} else {
					uint64_t v =
#ifndef PHP_WIN32
						(uint64_t) atoll((char *) p);
#else
						(uint64_t) _atoi64((char *) p);
#endif
					zend_bool uns = fields_metadata[i].flags & UNSIGNED_FLAG? TRUE:FALSE;
					/* We have to make it ASCIIZ temporarily */
#if SIZEOF_LONG==8
					if (uns == TRUE && v > 9223372036854775807L)
#elif SIZEOF_LONG==4
					if ((uns == TRUE && v > L64(2147483647)) ||
						(uns == FALSE && (( L64(2147483647) < (int64_t) v) ||
						(L64(-2147483648) > (int64_t) v))))
#else
#error Need fix for this architecture
#endif /* SIZEOF */
					{
						ZVAL_STRINGL(*current_field, (char *)p, len, 0);
					} else {
						ZVAL_LONG(*current_field, (long) v); /* the cast is safe */
					}
				}
				*(p + len) = save;
			} else if (as_int_or_float && perm_bind.php_type == IS_DOUBLE) {
				zend_uchar save = *(p + len);
				/* We have to make it ASCIIZ temporarily */
				*(p + len) = '\0';
				ZVAL_DOUBLE(*current_field, atof((char *) p));
				*(p + len) = save;
			} else
#endif /* MYSQLND_STRING_TO_INT_CONVERSION */
			if (fields_metadata[i].type == MYSQL_TYPE_BIT) {
				/*
				  BIT fields are specially handled. As they come as bit mask, we have
				  to convert it to human-readable representation. As the bits take
				  less space in the protocol than the numbers they represent, we don't
				  have enough space in the packet buffer to overwrite inside.
				  Thus, a bit more space is pre-allocated at the end of the buffer,
				  see php_mysqlnd_rowp_read(). And we add the strings at the end.
				  Definitely not nice, _hackish_ :(, but works.
				*/
				zend_uchar *start = bit_area;
				ps_fetch_from_1_to_8_bytes(*current_field, &(fields_metadata[i]), 0, &p, as_unicode, len TSRMLS_CC);
				/*
				  We have advanced in ps_fetch_from_1_to_8_bytes. We should go back because
				  later in this function there will be an advancement.
				*/
				p -= len;
				if (Z_TYPE_PP(current_field) == IS_LONG) {
					bit_area += 1 + sprintf((char *)start, "%ld", Z_LVAL_PP(current_field));
#if MYSQLND_UNICODE
					if (as_unicode) {
						ZVAL_UTF8_STRINGL(*current_field, start, bit_area - start - 1, 0);
					} else
#endif
					{
						ZVAL_STRINGL(*current_field, (char *) start, bit_area - start - 1, 0);
					}
				} else if (Z_TYPE_PP(current_field) == IS_STRING){
					memcpy(bit_area, Z_STRVAL_PP(current_field), Z_STRLEN_PP(current_field));
					bit_area += Z_STRLEN_PP(current_field);
					*bit_area++ = '\0';
					zval_dtor(*current_field);
#if MYSQLND_UNICODE
					if (as_unicode) {
						ZVAL_UTF8_STRINGL(*current_field, start, bit_area - start - 1, 0);
					} else
#endif
					{
						ZVAL_STRINGL(*current_field, (char *) start, bit_area - start - 1, 0);
					}
				}
				/*
				  IS_UNICODE should not be specially handled. In unicode mode
				  the buffers are not referenced - everything is copied.
				*/
			} else
#if MYSQLND_UNICODE == 0
			{
				ZVAL_STRINGL(*current_field, (char *)p, len, 0);
			}
#else
			/*
			  Here we have to convert to UTF16, which means not reusing the buffer.
			  Which in turn means that we can free the buffers once we have
			  stored the result set, if we use store_result().

			  Also the destruction of the zvals should not call zval_copy_ctor()
			  because then we will leak.

			  XXX: Keep in mind that up there there is an open `else` in
			  #ifdef MYSQLND_STRING_TO_INT_CONVERSION
			  which will make with this `if` an `else if`.
			*/
			if ((perm_bind.is_possibly_blob == TRUE &&
				 fields_metadata[i].charsetnr == MYSQLND_BINARY_CHARSET_NR) ||
				(!as_unicode && perm_bind.can_ret_as_str_in_uni == TRUE))
			{
				/* BLOB - no conversion please */
				ZVAL_STRINGL(*current_field, (char *)p, len, 0);
			} else {
				ZVAL_UTF8_STRINGL(*current_field, (char *)p, len, 0);
			}
#endif
			p += len;
			last_field_was_string = TRUE;
		}
	}
	if (last_field_was_string) {
		/* Normal queries: The buffer has one more byte at the end, because we need it */
		row_buffer->ptr[data_size] = '\0';
	}

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_read */
/*
  if normal statements => packet->fields is created by this function,
  if PS => packet->fields is passed from outside
*/
static enum_func_status
php_mysqlnd_rowp_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	MYSQLND_NET *net = conn->net;
	zend_uchar *p;
	enum_func_status ret = PASS;
	size_t old_chunk_size = net->stream->chunk_size;
	MYSQLND_PACKET_ROW *packet= (MYSQLND_PACKET_ROW *) _packet;
	size_t post_alloc_for_bit_fields = 0;
	size_t data_size = 0;

	DBG_ENTER("php_mysqlnd_rowp_read");

	if (!packet->binary_protocol && packet->bit_fields_count) {
		/* For every field we need terminating \0 */
		post_alloc_for_bit_fields = packet->bit_fields_total_len + packet->bit_fields_count;
	}

	ret = php_mysqlnd_read_row_ex(conn, packet->result_set_memory_pool, &packet->row_buffer, &data_size,
								  packet->persistent_alloc, post_alloc_for_bit_fields
								  TSRMLS_CC);
	if (FAIL == ret) {
		goto end;
	}
	MYSQLND_INC_CONN_STATISTIC_W_VALUE2(conn->stats, packet_type_to_statistic_byte_count[PROT_ROW_PACKET],
										MYSQLND_HEADER_SIZE + packet->header.size,
										packet_type_to_statistic_packet_count[PROT_ROW_PACKET],
										1);

	/* packet->row_buffer->ptr is of size 'data_size + 1' */
	packet->header.size = data_size;
	packet->row_buffer->app = data_size;

	if (ERROR_MARKER == (*(p = packet->row_buffer->ptr))) {
		/*
		   Error message as part of the result set,
		   not good but we should not hang. See:
		   Bug #27876 : SF with cyrillic variable name fails during execution
		*/
		ret = FAIL;
		php_mysqlnd_read_error_from_line(p + 1, data_size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										 TSRMLS_CC);
	} else if (EODATA_MARKER == *p && data_size < 8) { /* EOF */
		packet->eof = TRUE;
		p++;
		if (data_size > 1) {
			packet->warning_count = uint2korr(p);
			p += 2;
			packet->server_status = uint2korr(p);
			/* Seems we have 3 bytes reserved for future use */
			DBG_INF_FMT("server_status=%u warning_count=%u", packet->server_status, packet->warning_count);
		}
	} else {
		MYSQLND_INC_CONN_STATISTIC(conn->stats,
									packet->binary_protocol? STAT_ROWS_FETCHED_FROM_SERVER_PS:
															 STAT_ROWS_FETCHED_FROM_SERVER_NORMAL);

		packet->eof = FALSE;
		/* packet->field_count is set by the user of the packet */

		if (!packet->skip_extraction) {
			if (!packet->fields) {
				DBG_INF("Allocating packet->fields");
				/*
				  old-API will probably set packet->fields to NULL every time, though for
				  unbuffered sets it makes not much sense as the zvals in this buffer matter,
				  not the buffer. Constantly allocating and deallocating brings nothing.

				  For PS - if stmt_store() is performed, thus we don't have a cursor, it will
				  behave just like old-API buffered. Cursors will behave like a bit different,
				  but mostly like old-API unbuffered and thus will populate this array with
				  value.
				*/
				packet->fields = (zval **) mnd_pecalloc(packet->field_count, sizeof(zval *),
														packet->persistent_alloc);
			}
		} else {
			MYSQLND_INC_CONN_STATISTIC(conn->stats,
										packet->binary_protocol? STAT_ROWS_SKIPPED_PS:
																 STAT_ROWS_SKIPPED_NORMAL);
		}
	}

end:
	net->stream->chunk_size = old_chunk_size;
	DBG_RETURN(ret);
}
/* }}} */


/* {{{ php_mysqlnd_rowp_free_mem */
static void
php_mysqlnd_rowp_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_ROW *p;

	DBG_ENTER("php_mysqlnd_rowp_free_mem");
	p = (MYSQLND_PACKET_ROW *) _packet;
	if (p->row_buffer) {
		p->row_buffer->free_chunk(p->row_buffer TSRMLS_CC);
		p->row_buffer = NULL;
	}
	DBG_INF_FMT("stack_allocation=%u persistent=%u", (int)stack_allocation, (int)p->header.persistent);
	/*
	  Don't free packet->fields :
	  - normal queries -> store_result() | fetch_row_unbuffered() will transfer
	    the ownership and NULL it.
	  - PS will pass in it the bound variables, we have to use them! and of course
	    not free the array. As it is passed to us, we should not clean it ourselves.
	*/
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ php_mysqlnd_stats_read */
static enum_func_status
php_mysqlnd_stats_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	MYSQLND_PACKET_STATS *packet= (MYSQLND_PACKET_STATS *) _packet;
	size_t buf_len = conn->net->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;

	DBG_ENTER("php_mysqlnd_stats_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "statistics", PROT_STATS_PACKET);

	packet->message = mnd_emalloc(packet->header.size + 1);
	memcpy(packet->message, buf, packet->header.size);
	packet->message[packet->header.size] = '\0';
	packet->message_len = packet->header.size;

	DBG_RETURN(PASS);
}
/* }}} */


/* {{{ php_mysqlnd_stats_free_mem */
static
void php_mysqlnd_stats_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_STATS *p= (MYSQLND_PACKET_STATS *) _packet;
	if (p->message) {
		mnd_efree(p->message);
		p->message = NULL;
	}
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


/* 1 + 4 (id) + 2 (field_c) + 2 (param_c) + 1 (filler) + 2 (warnings ) */
#define PREPARE_RESPONSE_SIZE_41 9
#define PREPARE_RESPONSE_SIZE_50 12

/* {{{ php_mysqlnd_prepare_read */
static enum_func_status
php_mysqlnd_prepare_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/* In case of an error, we should have place to put it */
	size_t buf_len = conn->net->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	unsigned int data_size;
	MYSQLND_PACKET_PREPARE_RESPONSE *packet= (MYSQLND_PACKET_PREPARE_RESPONSE *) _packet;

	DBG_ENTER("php_mysqlnd_prepare_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "prepare", PROT_PREPARE_RESP_PACKET);
	BAIL_IF_NO_MORE_DATA;

	data_size = packet->header.size;
	packet->error_code = uint1korr(p);
	p++;
	BAIL_IF_NO_MORE_DATA;

	if (ERROR_MARKER == packet->error_code) {
		php_mysqlnd_read_error_from_line(p, data_size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										 TSRMLS_CC);
		DBG_RETURN(PASS);
	}

	if (data_size != PREPARE_RESPONSE_SIZE_41 &&
		data_size != PREPARE_RESPONSE_SIZE_50 &&
		!(data_size > PREPARE_RESPONSE_SIZE_50)) {
		DBG_ERR_FMT("Wrong COM_STMT_PREPARE response size. Received %u", data_size);
		php_error(E_WARNING, "Wrong COM_STMT_PREPARE response size. Received %u", data_size);
		DBG_RETURN(FAIL);
	}

	packet->stmt_id = uint4korr(p);
	p += 4;
	BAIL_IF_NO_MORE_DATA;

	/* Number of columns in result set */
	packet->field_count = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	packet->param_count = uint2korr(p);
	p += 2;
	BAIL_IF_NO_MORE_DATA;

	if (data_size > 9) {
		/* 0x0 filler sent by the server for 5.0+ clients */
		p++;
		BAIL_IF_NO_MORE_DATA;

		packet->warning_count = uint2korr(p);
	}

	DBG_INF_FMT("Prepare packet read: stmt_id=%u fields=%u params=%u",
				packet->stmt_id, packet->field_count, packet->param_count);

	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("PREPARE packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "PREPARE packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
					 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_prepare_free_mem */
static void
php_mysqlnd_prepare_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	MYSQLND_PACKET_PREPARE_RESPONSE *p= (MYSQLND_PACKET_PREPARE_RESPONSE *) _packet;
	if (!stack_allocation) {
		mnd_pefree(p, p->header.persistent);
	}
}
/* }}} */


/* {{{ php_mysqlnd_chg_user_read */
static enum_func_status
php_mysqlnd_chg_user_read(void *_packet, MYSQLND *conn TSRMLS_DC)
{
	/* There could be an error message */
	size_t buf_len = conn->net->cmd_buffer.length;
	zend_uchar *buf = (zend_uchar *) conn->net->cmd_buffer.buffer;
	zend_uchar *p = buf;
	zend_uchar *begin = buf;
	MYSQLND_PACKET_CHG_USER_RESPONSE *packet= (MYSQLND_PACKET_CHG_USER_RESPONSE *) _packet;

	DBG_ENTER("php_mysqlnd_chg_user_read");

	PACKET_READ_HEADER_AND_BODY(packet, conn, buf, buf_len, "change user response", PROT_CHG_USER_RESP_PACKET);
	BAIL_IF_NO_MORE_DATA;

	/*
	  Don't increment. First byte is ERROR_MARKER on error, but otherwise is starting byte
	  of encoded sequence for length.
	*/

	/* Should be always 0x0 or ERROR_MARKER for error */
	packet->field_count= uint1korr(p);
	p++;

	if (packet->header.size == 1 && buf[0] == EODATA_MARKER && packet->server_capabilities & CLIENT_SECURE_CONNECTION) {
		/* We don't handle 3.23 authentication */
		packet->server_asked_323_auth = TRUE;
		DBG_RETURN(FAIL);
	}

	if (ERROR_MARKER == packet->field_count) {
		php_mysqlnd_read_error_from_line(p, packet->header.size - 1,
										 packet->error_info.error,
										 sizeof(packet->error_info.error),
										 &packet->error_info.error_no,
										 packet->error_info.sqlstate
										 TSRMLS_CC);
	}
	BAIL_IF_NO_MORE_DATA;

	DBG_RETURN(PASS);
premature_end:
	DBG_ERR_FMT("CHANGE_USER packet %d bytes shorter than expected", p - begin - packet->header.size);
	php_error_docref(NULL TSRMLS_CC, E_WARNING, "CHANGE_USER packet "MYSQLND_SZ_T_SPEC" bytes shorter than expected",
						 p - begin - packet->header.size);
	DBG_RETURN(FAIL);
}
/* }}} */


/* {{{ php_mysqlnd_chg_user_free_mem */
static void
php_mysqlnd_chg_user_free_mem(void *_packet, zend_bool stack_allocation TSRMLS_DC)
{
	if (!stack_allocation) {
		mnd_pefree(_packet, ((MYSQLND_PACKET_CHG_USER_RESPONSE *)_packet)->header.persistent);
	}
}
/* }}} */


/* {{{ packet_methods */
static
mysqlnd_packet_methods packet_methods[PROT_LAST] =
{
	{
		sizeof(MYSQLND_PACKET_GREET),
		php_mysqlnd_greet_read,
		NULL, /* write */
		php_mysqlnd_greet_free_mem,
	}, /* PROT_GREET_PACKET */
	{
		sizeof(MYSQLND_PACKET_AUTH),
		NULL, /* read */
		php_mysqlnd_auth_write,
		php_mysqlnd_auth_free_mem,
	}, /* PROT_AUTH_PACKET */
	{
		sizeof(MYSQLND_PACKET_OK),
		php_mysqlnd_ok_read, /* read */
		NULL, /* write */
		php_mysqlnd_ok_free_mem,
	}, /* PROT_OK_PACKET */
	{
		sizeof(MYSQLND_PACKET_EOF),
		php_mysqlnd_eof_read, /* read */
		NULL, /* write */
		php_mysqlnd_eof_free_mem,
	}, /* PROT_EOF_PACKET */
	{
		sizeof(MYSQLND_PACKET_COMMAND),
		NULL, /* read */
		php_mysqlnd_cmd_write, /* write */
		php_mysqlnd_cmd_free_mem,
	}, /* PROT_CMD_PACKET */
	{
		sizeof(MYSQLND_PACKET_RSET_HEADER),
		php_mysqlnd_rset_header_read, /* read */
		NULL, /* write */
		php_mysqlnd_rset_header_free_mem,
	}, /* PROT_RSET_HEADER_PACKET */
	{
		sizeof(MYSQLND_PACKET_RES_FIELD),
		php_mysqlnd_rset_field_read, /* read */
		NULL, /* write */
		php_mysqlnd_rset_field_free_mem,
	}, /* PROT_RSET_FLD_PACKET */
	{
		sizeof(MYSQLND_PACKET_ROW),
		php_mysqlnd_rowp_read, /* read */
		NULL, /* write */
		php_mysqlnd_rowp_free_mem,
	}, /* PROT_ROW_PACKET */
	{
		sizeof(MYSQLND_PACKET_STATS),
		php_mysqlnd_stats_read, /* read */
		NULL, /* write */
		php_mysqlnd_stats_free_mem,
	}, /* PROT_STATS_PACKET */
	{
		sizeof(MYSQLND_PACKET_PREPARE_RESPONSE),
		php_mysqlnd_prepare_read, /* read */
		NULL, /* write */
		php_mysqlnd_prepare_free_mem,
	}, /* PROT_PREPARE_RESP_PACKET */
	{
		sizeof(MYSQLND_PACKET_CHG_USER_RESPONSE),
		php_mysqlnd_chg_user_read, /* read */
		NULL, /* write */
		php_mysqlnd_chg_user_free_mem,
	} /* PROT_CHG_USER_RESP_PACKET */
};
/* }}} */


/* {{{ mysqlnd_protocol::get_greet_packet */
static struct st_mysqlnd_packet_greet *
MYSQLND_METHOD(mysqlnd_protocol, get_greet_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_greet * packet = mnd_pecalloc(1, packet_methods[PROT_GREET_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_greet_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_GREET_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_auth_packet */
static struct st_mysqlnd_packet_auth *
MYSQLND_METHOD(mysqlnd_protocol, get_auth_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_auth * packet = mnd_pecalloc(1, packet_methods[PROT_AUTH_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_auth_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_AUTH_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_ok_packet */
static struct st_mysqlnd_packet_ok *
MYSQLND_METHOD(mysqlnd_protocol, get_ok_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_ok * packet = mnd_pecalloc(1, packet_methods[PROT_OK_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_ok_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_OK_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_eof_packet */
static struct st_mysqlnd_packet_eof *
MYSQLND_METHOD(mysqlnd_protocol, get_eof_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_eof * packet = mnd_pecalloc(1, packet_methods[PROT_EOF_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_eof_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_EOF_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_command_packet */
static struct st_mysqlnd_packet_command *
MYSQLND_METHOD(mysqlnd_protocol, get_command_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_command * packet = mnd_pecalloc(1, packet_methods[PROT_CMD_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_command_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_CMD_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_rset_packet */
static struct st_mysqlnd_packet_rset_header *
MYSQLND_METHOD(mysqlnd_protocol, get_rset_header_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_rset_header * packet = mnd_pecalloc(1, packet_methods[PROT_RSET_HEADER_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_rset_header_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_RSET_HEADER_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_result_field_packet */
static struct st_mysqlnd_packet_res_field *
MYSQLND_METHOD(mysqlnd_protocol, get_result_field_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_res_field * packet = mnd_pecalloc(1, packet_methods[PROT_RSET_FLD_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_result_field_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_RSET_FLD_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_row_packet */
static struct st_mysqlnd_packet_row *
MYSQLND_METHOD(mysqlnd_protocol, get_row_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_row * packet = mnd_pecalloc(1, packet_methods[PROT_ROW_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_row_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_ROW_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_stats_packet */
static struct st_mysqlnd_packet_stats *
MYSQLND_METHOD(mysqlnd_protocol, get_stats_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_stats * packet = mnd_pecalloc(1, packet_methods[PROT_STATS_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_stats_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_STATS_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_prepare_response_packet */
static struct st_mysqlnd_packet_prepare_response *
MYSQLND_METHOD(mysqlnd_protocol, get_prepare_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_prepare_response * packet = mnd_pecalloc(1, packet_methods[PROT_PREPARE_RESP_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_prepare_response_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_PREPARE_RESP_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


/* {{{ mysqlnd_protocol::get_change_user_response_packet */
static struct st_mysqlnd_packet_chg_user_resp*
MYSQLND_METHOD(mysqlnd_protocol, get_change_user_response_packet)(MYSQLND_PROTOCOL * const protocol, zend_bool persistent TSRMLS_DC)
{
	struct st_mysqlnd_packet_chg_user_resp * packet = mnd_pecalloc(1, packet_methods[PROT_CHG_USER_RESP_PACKET].struct_size, persistent);
	DBG_ENTER("mysqlnd_protocol::get_change_user_response_packet");
	if (packet) {
		packet->header.m = &packet_methods[PROT_CHG_USER_RESP_PACKET];
		packet->header.persistent = persistent;
	}
	DBG_RETURN(packet);
}
/* }}} */


static
MYSQLND_CLASS_METHODS_START(mysqlnd_protocol)
	MYSQLND_METHOD(mysqlnd_protocol, get_greet_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_auth_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_ok_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_command_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_eof_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_rset_header_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_result_field_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_row_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_stats_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_prepare_response_packet),
	MYSQLND_METHOD(mysqlnd_protocol, get_change_user_response_packet)
MYSQLND_CLASS_METHODS_END;


/* {{{ mysqlnd_protocol_init */
PHPAPI MYSQLND_PROTOCOL *
mysqlnd_protocol_init(zend_bool persistent TSRMLS_DC)
{
	size_t alloc_size = sizeof(MYSQLND_PROTOCOL) + mysqlnd_plugin_count() * sizeof(void *);
	MYSQLND_PROTOCOL *ret = mnd_pecalloc(1, alloc_size, persistent);

	DBG_ENTER("mysqlnd_protocol_init");
	DBG_INF_FMT("persistent=%u", persistent);
	if (ret) {
		ret->persistent = persistent;
		ret->m = mysqlnd_mysqlnd_protocol_methods;
	}

	DBG_RETURN(ret);
}
/* }}} */


/* {{{ mysqlnd_protocol_free */
PHPAPI void
mysqlnd_protocol_free(MYSQLND_PROTOCOL * const protocol TSRMLS_DC)
{
	DBG_ENTER("mysqlnd_protocol_free");

	if (protocol) {
		zend_bool pers = protocol->persistent;
		mnd_pefree(protocol, pers);
	}
	DBG_VOID_RETURN;
}
/* }}} */


/* {{{ _mysqlnd_plugin_get_plugin_protocol_data */
PHPAPI void **
_mysqlnd_plugin_get_plugin_protocol_data(const MYSQLND_PROTOCOL * protocol, unsigned int plugin_id TSRMLS_DC)
{
	DBG_ENTER("_mysqlnd_plugin_get_plugin_protocol_data");
	DBG_INF_FMT("plugin_id=%u", plugin_id);
	if (!protocol || plugin_id >= mysqlnd_plugin_count()) {
		return NULL;
	}
	DBG_RETURN((void *)((char *)protocol + sizeof(MYSQLND_PROTOCOL) + plugin_id * sizeof(void *)));
}
/* }}} */


/* {{{ mysqlnd_protocol_get_methods */
PHPAPI struct st_mysqlnd_protocol_methods *
mysqlnd_protocol_get_methods()
{
	return &mysqlnd_mysqlnd_protocol_methods;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
