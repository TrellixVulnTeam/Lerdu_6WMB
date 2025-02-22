//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2009. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_OFFSET_PTR_HPP
#define BOOST_OFFSET_PTR_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>

#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/detail/utilities.hpp>
#include <boost/interprocess/detail/cast_tags.hpp>
#include <boost/interprocess/detail/mpl.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/pointer_to_other.hpp>
#include <boost/assert.hpp>
#include <ostream>
#include <istream>
#include <iterator>
#include <boost/aligned_storage.hpp>
#include <boost/type_traits/alignment_of.hpp>

//!\file
//!Describes a smart pointer that stores the offset between this pointer and
//!target pointee, called offset_ptr.

namespace boost {

//Predeclarations
template <class T>
struct has_trivial_constructor;

template <class T>
struct has_trivial_destructor;

namespace interprocess {

//!A smart pointer that stores the offset between between the pointer and the
//!the object it points. This allows offset allows special properties, since
//!the pointer is independent from the address address of the pointee, if the
//!pointer and the pointee are still separated by the same offset. This feature
//!converts offset_ptr in a smart pointer that can be placed in shared memory and
//!memory mapped files mapped in different addresses in every process.
template <class PointedType, class DifferenceType, class OffsetType, std::size_t OffsetAlignment>
class offset_ptr
{
   /// @cond
   typedef offset_ptr<PointedType, DifferenceType, OffsetType, OffsetAlignment>   self_t;

   void unspecified_bool_type_func() const {}
   typedef void (self_t::*unspecified_bool_type)() const;

   //Note: using the address of a local variable to point to another address
   //is not standard conforming and this can be optimized-away by the compiler.
   //Non-inlining is a method to remain illegal and correct
   #if defined(_MSC_VER) && (_MSC_VER >= 1400)
   __declspec(noinline) //this workaround is needed for msvc-8.0 and msvc-9.0
   #elif defined (__GNUC__)//this workaround is needed for GCC
   __attribute__((__noinline__))
   #endif
   void set_offset(const PointedType *ptr)
   {
      #if defined (__GNUC__)
      //asm(""); //Prevents the function to be optimized-away (provokes an special "side-effect")
      #endif
      //offset == 1 && ptr != 0 is not legal for this pointer
      if(!ptr){
         internal.m_offset = 1;
      }
      else{
         internal.m_offset = (OffsetType)((const char*)ptr - (const char*)(this));
         BOOST_ASSERT(internal.m_offset != 1);
      }
   }

   #if defined(_MSC_VER) && (_MSC_VER >= 1400)
   __declspec(noinline)
   #elif defined (__GNUC__)
   __attribute__((__noinline__))
   #endif
   PointedType * get_pointer() const
   {
      #if defined (__GNUC__)
      //asm(""); //Prevents the function to be optimized-away (provokes an special "side-effect")
      #endif
      return static_cast<PointedType *>(
         static_cast<void*>(
            (internal.m_offset == 1) ? 
               0 : 
                  (const_cast<char*>(reinterpret_cast<const char*>(this)) + internal.m_offset)
         )
      );
   }

   void inc_offset(DifferenceType bytes)
   {  internal.m_offset += bytes;   }

   void dec_offset(DifferenceType bytes)
   {  internal.m_offset -= bytes;   }

   union internal_type{
      OffsetType m_offset; //Distance between this object and pointed address
    typename ::boost::aligned_storage
         < sizeof(OffsetType)
         , (OffsetAlignment == offset_type_alignment) ?
            ::boost::alignment_of<OffsetType>::value : OffsetAlignment
         >::type alignment_helper;
   } internal;
   /// @endcond

   public:
   typedef PointedType *                     pointer;
   typedef typename ipcdetail::
      add_reference<PointedType>::type       reference;
   typedef PointedType                       value_type;
   typedef DifferenceType                    difference_type;
   typedef std::random_access_iterator_tag   iterator_category;
   typedef OffsetType                        offset_type;

   public:   //Public Functions

   //!Constructor from raw pointer (allows "0" pointer conversion).
   //!Never throws.
   offset_ptr(pointer ptr = 0) {  this->set_offset(ptr); }

   //!Constructor from other pointer.
   //!Never throws.
   template <class T>
   offset_ptr(T *ptr) 
   {  pointer p (ptr);  (void)p; this->set_offset(p); }

   //!Constructor from other offset_ptr
   //!Never throws.
   offset_ptr(const offset_ptr& ptr) 
   {  this->set_offset(ptr.get());   }

   //!Constructor from other offset_ptr. If pointers of pointee types are 
   //!convertible, offset_ptrs will be convertibles. Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   offset_ptr(const offset_ptr<T2, P2, O2, A2> &ptr) 
   {  pointer p(ptr.get());  (void)p; this->set_offset(p);   }

   //!Emulates static_cast operator.
   //!Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   offset_ptr(const offset_ptr<T2, P2, O2, A2> & r, ipcdetail::static_cast_tag)
   {  this->set_offset(static_cast<PointedType*>(r.get()));   }

   //!Emulates const_cast operator.
   //!Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   offset_ptr(const offset_ptr<T2, P2, O2, A2> & r, ipcdetail::const_cast_tag)
   {  this->set_offset(const_cast<PointedType*>(r.get()));   }

   //!Emulates dynamic_cast operator.
   //!Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   offset_ptr(const offset_ptr<T2, P2, O2, A2> & r, ipcdetail::dynamic_cast_tag)
   {  this->set_offset(dynamic_cast<PointedType*>(r.get()));   }

   //!Emulates reinterpret_cast operator.
   //!Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   offset_ptr(const offset_ptr<T2, P2, O2, A2> & r, ipcdetail::reinterpret_cast_tag)
   {  this->set_offset(reinterpret_cast<PointedType*>(r.get()));   }

   //!Obtains raw pointer from offset.
   //!Never throws.
   pointer get()const
   {  return this->get_pointer();   }

   offset_type get_offset() const
   {  return internal.m_offset;  }

   //!Pointer-like -> operator. It can return 0 pointer.
   //!Never throws.
   pointer operator->() const           
   {  return this->get(); }

   //!Dereferencing operator, if it is a null offset_ptr behavior 
   //!   is undefined. Never throws.
   reference operator* () const           
   {
      pointer p = this->get();
      reference r = *p;
      return r;
   }

   //!Indexing operator.
   //!Never throws.
   template<class T>
   reference operator[](T idx) const   
   {  return this->get()[idx];  }

   //!Assignment from pointer (saves extra conversion).
   //!Never throws.
   offset_ptr& operator= (pointer from)
   {  this->set_offset(from); return *this;  }

   //!Assignment from other offset_ptr.
   //!Never throws.
   offset_ptr& operator= (const offset_ptr & pt)
   {  pointer p(pt.get());  (void)p; this->set_offset(p);  return *this;  }

   //!Assignment from related offset_ptr. If pointers of pointee types 
   //!   are assignable, offset_ptrs will be assignable. Never throws.
   template<class T2, class P2, class O2, std::size_t A2>
   offset_ptr& operator= (const offset_ptr<T2, P2, O2, A2> & pt)
   {  pointer p(pt.get()); this->set_offset(p);  return *this;  }
 
   //!offset_ptr + difference_type.
   //!Never throws.
   template<class T>
   offset_ptr operator+ (T offset) const   
   {  return offset_ptr(this->get()+offset);   }

   //!offset_ptr - difference_type.
   //!Never throws.
   template<class T>
   offset_ptr operator- (T offset) const   
   {  return offset_ptr(this->get()-offset);   }

   //!offset_ptr += difference_type.
   //!Never throws.
   template<class T>
   offset_ptr &operator+= (T offset)
   {  this->inc_offset(offset * sizeof (PointedType));   return *this;  }

   //!offset_ptr -= difference_type.
   //!Never throws.
   template<class T>
   offset_ptr &operator-= (T offset)
   {  this->dec_offset(offset * sizeof (PointedType));   return *this;  }

   //!++offset_ptr.
   //!Never throws.
   offset_ptr& operator++ (void) 
   {  this->inc_offset(sizeof (PointedType));   return *this;  }

   //!offset_ptr++.
   //!Never throws.
   offset_ptr operator++ (int)
   {  offset_ptr temp(*this); ++*this; return temp; }

   //!--offset_ptr.
   //!Never throws.
   offset_ptr& operator-- (void) 
   {  this->dec_offset(sizeof (PointedType));   return *this;  }

   //!offset_ptr--.
   //!Never throws.
   offset_ptr operator-- (int)
   {  offset_ptr temp(*this); --*this; return temp; }

   //!safe bool conversion operator.
   //!Never throws.
   operator unspecified_bool_type() const  
   {  return this->get()? &self_t::unspecified_bool_type_func : 0;   }

   //!Not operator. Not needed in theory, but improves portability. 
   //!Never throws
   bool operator! () const
   {  return this->get() == 0;   }
/*
   friend void swap (offset_ptr &pt, offset_ptr &pt2)
   {  
      value_type *ptr = pt.get();
      pt = pt2;
      pt2 = ptr;
   }
*/
};

//!offset_ptr<T1, P1, O1, A1> == offset_ptr<T2, P2, O2, A2>.
//!Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline bool operator== (const offset_ptr<T1, P1, O1, A1> &pt1, 
                        const offset_ptr<T2, P2, O2, A2> &pt2)
{  return pt1.get() == pt2.get();  }

//!offset_ptr<T1, P1, O1, A1> != offset_ptr<T2, P2, O2, A2>.
//!Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline bool operator!= (const offset_ptr<T1, P1, O1, A1> &pt1, 
                        const offset_ptr<T2, P2, O2, A2> &pt2)
{  return pt1.get() != pt2.get();  }

//!offset_ptr<T1, P1, O1, A1> < offset_ptr<T2, P2, O2, A2>.
//!Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline bool operator< (const offset_ptr<T1, P1, O1, A1> &pt1, 
                       const offset_ptr<T2, P2, O2, A2> &pt2)
{  return pt1.get() < pt2.get();  }

//!offset_ptr<T1, P1, O1, A1> <= offset_ptr<T2, P2, O2, A2>.
//!Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline bool operator<= (const offset_ptr<T1, P1, O1, A1> &pt1, 
                        const offset_ptr<T2, P2, O2, A2> &pt2)
{  return pt1.get() <= pt2.get();  }

//!offset_ptr<T1, P1, O1, A1> > offset_ptr<T2, P2, O2, A2>.
//!Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline bool operator>  (const offset_ptr<T1, P1, O1, A1> &pt1, 
                        const offset_ptr<T2, P2, O2, A2> &pt2)
{  return pt1.get() > pt2.get();  }

//!offset_ptr<T1, P1, O1, A1> >= offset_ptr<T2, P2, O2, A2>.
//!Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline bool operator>= (const offset_ptr<T1, P1, O1, A1> &pt1, 
                        const offset_ptr<T2, P2, O2, A2> &pt2)
{  return pt1.get() >= pt2.get();  }

//!operator<<
//!for offset ptr
template<class E, class T, class W, class X, class Y, std::size_t Z> 
inline std::basic_ostream<E, T> & operator<< 
   (std::basic_ostream<E, T> & os, offset_ptr<W, X, Y, Z> const & p)
{  return os << p.get_offset();   }

//!operator>> 
//!for offset ptr
template<class E, class T, class W, class X, class Y, std::size_t Z> 
inline std::basic_istream<E, T> & operator>> 
   (std::basic_istream<E, T> & is, offset_ptr<W, X, Y, Z> & p)
{  return is >> p.get_offset();  }

//!difference_type + offset_ptr
//!operation
template<class T, class P, class O, std::size_t A, class D>
inline offset_ptr<T, P, O, A> operator+(D diff, const offset_ptr<T, P, O, A>& right)
{  return right + diff;  }

//!offset_ptr - offset_ptr
//!operation
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline typename offset_ptr<T1, P1, O1, A1>::difference_type operator-
   (const offset_ptr<T1, P1, O1, A1> &pt, const offset_ptr<T2, P2, O2, A2> &pt2)
{  return typename offset_ptr<T1, P1, O1, A1>::difference_type(pt.get()- pt2.get());   }

//!swap specialization 
//!for offset_ptr
template<class T, class P, class O, std::size_t A>
inline void swap (boost::interprocess::offset_ptr<T, P, O, A> &pt, 
                  boost::interprocess::offset_ptr<T, P, O, A> &pt2)
{  
   typename offset_ptr<T, P, O, A>::value_type *ptr = pt.get();
   pt = pt2;
   pt2 = ptr;
}

//!Simulation of static_cast between pointers. Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline boost::interprocess::offset_ptr<T1, P1, O1, A1> 
   static_pointer_cast(const boost::interprocess::offset_ptr<T2, P2, O2, A2> & r)
{  
   return boost::interprocess::offset_ptr<T1, P1, O1, A1>
            (r, boost::interprocess::ipcdetail::static_cast_tag());  
}

//!Simulation of const_cast between pointers. Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline boost::interprocess::offset_ptr<T1, P1, O1, A1>
   const_pointer_cast(const boost::interprocess::offset_ptr<T2, P2, O2, A2> & r)
{  
   return boost::interprocess::offset_ptr<T1, P1, O1, A1>
            (r, boost::interprocess::ipcdetail::const_cast_tag());  
}

//!Simulation of dynamic_cast between pointers. Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline boost::interprocess::offset_ptr<T1, P1, O1, A1> 
   dynamic_pointer_cast(const boost::interprocess::offset_ptr<T2, P2, O2, A2> & r)
{  
   return boost::interprocess::offset_ptr<T1, P1, O1, A1>
            (r, boost::interprocess::ipcdetail::dynamic_cast_tag());  
}

//!Simulation of reinterpret_cast between pointers. Never throws.
template<class T1, class P1, class O1, std::size_t A1, class T2, class P2, class O2, std::size_t A2>
inline boost::interprocess::offset_ptr<T1, P1, O1, A1>
   reinterpret_pointer_cast(const boost::interprocess::offset_ptr<T2, P2, O2, A2> & r)
{  
   return boost::interprocess::offset_ptr<T1, P1, O1, A1>
            (r, boost::interprocess::ipcdetail::reinterpret_cast_tag());  
}

}  //namespace interprocess {

/// @cond

//!has_trivial_constructor<> == true_type specialization for optimizations
template <class T, class P, class O, std::size_t A>
struct has_trivial_constructor< boost::interprocess::offset_ptr<T, P, O, A> > 
{
   enum { value = true };
};

///has_trivial_destructor<> == true_type specialization for optimizations
template <class T, class P, class O, std::size_t A>
struct has_trivial_destructor< boost::interprocess::offset_ptr<T, P, O, A> > 
{
   enum { value = true };
};

//#if !defined(_MSC_VER) || (_MSC_VER >= 1400)
namespace interprocess {
//#endif
//!get_pointer() enables boost::mem_fn to recognize offset_ptr. 
//!Never throws.
template <class T, class P, class O, std::size_t A>
inline T * get_pointer(boost::interprocess::offset_ptr<T, P, O, A> const & p)
{  return p.get();   }
//#if !defined(_MSC_VER) || (_MSC_VER >= 1400)
}  //namespace interprocess
//#endif

/// @endcond
}  //namespace boost {

/// @cond

namespace boost{

//This is to support embedding a bit in the pointer
//for intrusive containers, saving space
namespace intrusive {

//Predeclaration to avoid including header
template<class VoidPointer, std::size_t N>
struct max_pointer_plus_bits;

template<std::size_t OffsetAlignment, class P, class O, std::size_t A>
struct max_pointer_plus_bits<boost::interprocess::offset_ptr<void, P, O, A>, OffsetAlignment>
{
   //The offset ptr can embed one bit less than the alignment since it
   //uses offset == 1 to store the null pointer.
   static const std::size_t value = ::boost::interprocess::ipcdetail::ls_zeros<OffsetAlignment>::value - 1;
};

//Predeclaration
template<class Pointer, std::size_t NumBits>
struct pointer_plus_bits;

template<class T, class P, class O, std::size_t A, std::size_t NumBits>
struct pointer_plus_bits<boost::interprocess::offset_ptr<T, P, O, A>, NumBits>
{
   typedef boost::interprocess::offset_ptr<T, P, O, A>         pointer;
   //Bits are stored in the lower bits of the pointer except the LSB,
   //because this bit is used to represent the null pointer.
   static const std::size_t Mask = ((std::size_t(1) << NumBits)-1)<<1u; 

   static pointer get_pointer(const pointer &n)
   {  return reinterpret_cast<T*>(std::size_t(n.get()) & ~std::size_t(Mask));  }

   static void set_pointer(pointer &n, pointer p)
   {
      std::size_t pint = std::size_t(p.get());
      BOOST_ASSERT(0 == (std::size_t(pint) & Mask));
      n = reinterpret_cast<T*>(pint | (std::size_t(n.get()) & std::size_t(Mask)));
   }

   static std::size_t get_bits(const pointer &n)
   {  return(std::size_t(n.get()) & std::size_t(Mask)) >> 1u;  }

   static void set_bits(pointer &n, std::size_t b)
   {
      BOOST_ASSERT(b < (std::size_t(1) << NumBits));
      n = reinterpret_cast<T*>(std::size_t(get_pointer(n).get()) | (b << 1u));
   }
};

}  //namespace intrusive

template<class T, class T2, class T3, std::size_t A, class U>
struct pointer_to_other< ::boost::interprocess::offset_ptr<T, T2, T3, A>, U >
{
   typedef ::boost::interprocess::offset_ptr<U, T2, T3, A> type;
};

}  //namespace boost{
/// @endcond

#include <boost/interprocess/detail/config_end.hpp>

#endif //#ifndef BOOST_OFFSET_PTR_HPP

