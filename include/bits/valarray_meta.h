// The template and inlines for the -*- C++ -*- internal _Meta class.

// Copyright (C) 1997-1999 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

// Written by Gabriel Dos Reis <Gabriel.Dos-Reis@cmla.ens-cachan.fr>

#ifndef _CPP_VALARRAY_META_H
#define _CPP_VALARRAY_META_H 1

namespace std {

    //
    // Implementing a loosened valarray return value is tricky.
    // First we need to meet 26.3.1/3: we should not add more than
    // two levels of template nesting. Therefore we resort to template
    // template to "flatten" loosened return value types.
    // At some point we use partial specialization to remove one level
    // template nesting due to _Expr<>
    //
    

    // This class is NOT defined. It doesn't need to.
    template<typename _Tp1, typename _Tp2> class _Constant;

    //
    // Unary function application closure.
    //
    template<class _Dom> class _UnFunBase {
    public:
        typedef typename _Dom::value_type value_type;
        typedef value_type _Vt;
        
        _UnFunBase (const _Dom& __e, _Vt __f(_Vt))
                : _M_expr(__e), _M_func(__f) {}
        
        _Vt operator[] (size_t __i) const { return _M_func(_M_expr[__i]); }
        size_t size () const { return _M_expr.size(); }
        
    private:
        const _Dom& _M_expr;
        _Vt (*_M_func)(_Vt);
    };

    template<template<class, class> class _Meta, class _Dom>
        class _UnFunClos;
    
    template<class _Dom>
    struct _UnFunClos<_Expr,_Dom> : _UnFunBase<_Dom> {
        typedef _UnFunBase<_Dom> _Base;
        typedef typename _Base::value_type value_type;
        
        _UnFunClos (const _Dom& __e, value_type __f(value_type))
                : _Base (__e, __f) {}
    };
    
    template<typename _Tp>
    struct _UnFunClos<_ValArray,_Tp> : _UnFunBase<valarray<_Tp> > {
        typedef _UnFunBase<valarray<_Tp> > _Base;
        typedef typename _Base::value_type value_type;
        
        _UnFunClos (const valarray<_Tp>& __v, _Tp __f(_Tp))
                : _Base (__v, __f) {}
    };

    //
    // Binary function application closure.
    //
    template<template<class, class> class _Meta1,
        template<class, class> class Meta2,
        class _Dom1, class _Dom2> class _BinFunClos;
    
    template<class _Dom1, class _Dom2> class _BinFunBase {
    public:
        typedef typename _Dom1::value_type value_type;
        typedef value_type _Vt;

        _BinFunBase (const _Dom1& __e1, const _Dom2& __e2,
                      _Vt __f (_Vt, _Vt))
                : _M_expr1 (__e1), _M_expr2 (__e2), _M_func (__f) {}

        value_type operator[] (size_t __i) const
        { return _M_func (_M_expr1[__i], _M_expr2[__i]); }
        size_t size () const { return _M_expr1.size (); }

    private:
        const _Dom1& _M_expr1;
        const _Dom2& _M_expr2;
        _Vt (*_M_func)(_Vt, _Vt);
    };

    template<class _Dom> class _BinFunBase1 {
    public:
        typedef typename _Dom::value_type value_type ;
        typedef value_type _Vt;

        _BinFunBase1 (const _Vt& __c, const _Dom& __e, _Vt __f(_Vt, _Vt))
                : _M_expr1 (__c), _M_expr2 (__e), _M_func (__f) {}

        value_type operator[] (size_t __i) const
        { return _M_func (_M_expr1, _M_expr2[__i]); }
        size_t size () const { return _M_expr2.size (); }

    private:
        const _Vt& _M_expr1;
        const _Dom& _M_expr2;
        _Vt (*_M_func)(_Vt, _Vt);
    };

    template<class _Dom> class _BinFunBase2 {
    public:
        typedef typename _Dom::value_type value_type;
        typedef value_type _Vt;

        _BinFunBase2 (const _Dom& __e, const _Vt& __c, _Vt __f(_Vt, _Vt))
                : _M_expr1 (__e), _M_expr2 (__c), _M_func (__f) {}

        value_type operator[] (size_t __i) const
        { return _M_func (_M_expr1[__i], _M_expr2); }
        size_t size () const { return _M_expr1.size (); }

    private:
        const _Dom& _M_expr1;
        const _Vt& _M_expr2;
        _Vt (*_M_func)(_Vt, _Vt);
    };

    template<class _Dom1, class _Dom2>
    struct _BinFunClos<_Expr,_Expr,_Dom1,_Dom2> : _BinFunBase<_Dom1,_Dom2> {
        typedef _BinFunBase<_Dom1,_Dom2> _Base;
        typedef typename _Base::value_type value_type;
        typedef value_type _Tp;

        _BinFunClos (const _Dom1& __e1, const _Dom2& __e2,
                     _Tp __f(_Tp, _Tp))
                : _Base (__e1, __e2, __f) {}
    };

    template<typename _Tp>
    struct _BinFunClos<_ValArray,_ValArray,_Tp,_Tp>
        : _BinFunBase<valarray<_Tp>, valarray<_Tp> > {
        typedef _BinFunBase<valarray<_Tp>, valarray<_Tp> > _Base;
        typedef _Tp value_type;

        _BinFunClos (const valarray<_Tp>& __v, const valarray<_Tp>& __w,
                     _Tp __f(_Tp, _Tp))
                : _Base (__v, __w, __f) {}
    };
    
    template<class _Dom>
    struct _BinFunClos<_Expr,_ValArray,_Dom,typename _Dom::value_type>
        : _BinFunBase<_Dom,valarray<typename _Dom::value_type> > {
        typedef typename _Dom::value_type _Tp;
        typedef _BinFunBase<_Dom,valarray<_Tp> > _Base;
        typedef _Tp value_type;

        _BinFunClos (const _Dom& __e, const valarray<_Tp>& __v,
                     _Tp __f(_Tp, _Tp))
                : _Base (__e, __v, __f) {}
    };

    template<class _Dom>
    struct _BinFunClos<_ValArray,_Expr,typename _Dom::value_type,_Dom>
        : _BinFunBase<valarray<typename _Dom::value_type>,_Dom> {
        typedef typename _Dom::value_type _Tp;
        typedef _BinFunBase<_Dom,valarray<_Tp> > _Base;
        typedef _Tp value_type;

        _BinFunClos (const valarray<_Tp>& __v, const _Dom& __e,
                     _Tp __f(_Tp, _Tp))
                : _Base (__v, __e, __f) {}
    };

    template<class _Dom>
    struct _BinFunClos<_Expr,_Constant,_Dom,typename _Dom::value_type>
        : _BinFunBase2<_Dom> {
        typedef typename _Dom::value_type _Tp;
        typedef _Tp value_type;
        typedef _BinFunBase2<_Dom> _Base;

        _BinFunClos (const _Dom& __e, const _Tp& __t, _Tp __f (_Tp, _Tp))
                : _Base (__e, __t, __f) {}
    };

    template<class _Dom>
    struct _BinFunClos<_Constant,_Expr,_Dom,typename _Dom::value_type>
        : _BinFunBase1<_Dom> {
        typedef typename _Dom::value_type _Tp;
        typedef _Tp value_type;
        typedef _BinFunBase1<_Dom> _Base;

        _BinFunClos (const _Tp& __t, const _Dom& __e, _Tp __f (_Tp, _Tp))
                : _Base (__t, __e, __f) {}
    };

    template<typename _Tp>
    struct _BinFunClos<_ValArray,_Constant,_Tp,_Tp>
        : _BinFunBase2<valarray<_Tp> > {
        typedef _BinFunBase2<valarray<_Tp> > _Base;
        typedef _Tp value_type;

        _BinFunClos (const valarray<_Tp>& __v, const _Tp& __t,
                     _Tp __f(_Tp, _Tp))
                : _Base (__v, __t, __f) {}
    };

    template<typename _Tp>
    struct _BinFunClos<_Constant,_ValArray,_Tp,_Tp>
        : _BinFunBase1<valarray<_Tp> > {
        typedef _BinFunBase1<valarray<_Tp> > _Base;
        typedef _Tp value_type;

        _BinFunClos (const _Tp& __t, const valarray<_Tp>& __v,
                     _Tp __f (_Tp, _Tp))
                : _Base (__t, __v, __f) {}
    };

    //
    // Apply function taking a value/const reference closure
    //

    template<typename _Dom, typename _Arg> class _FunBase {
    public:
        typedef typename _Dom::value_type value_type;

        _FunBase (const _Dom& __e, value_type __f(_Arg))
                : _M_expr (__e), _M_func (__f) {}

        value_type operator[] (size_t __i) const
        { return _M_func (_M_expr[__i]); }
        size_t size() const { return _M_expr.size ();}

    private:
        const _Dom& _M_expr;
        value_type (*_M_func)(_Arg);
    };

    template<class _Dom>
    struct _ValFunClos<_Expr,_Dom>
        : _FunBase<_Dom, typename _Dom::value_type> {
        typedef _FunBase<_Dom, typename _Dom::value_type> _Base;
        typedef typename _Base::value_type value_type;
        typedef value_type _Tp;
    
        _ValFunClos (const _Dom& __e, _Tp __f (_Tp)) : _Base (__e, __f) {}
    };

    template<typename _Tp>
    struct _ValFunClos<_ValArray,_Tp>
        : _FunBase<valarray<_Tp>, _Tp> {
        typedef _FunBase<valarray<_Tp>, _Tp> _Base;
        typedef _Tp value_type;

        _ValFunClos (const valarray<_Tp>& __v, _Tp __f(_Tp))
                : _Base (__v, __f) {}
    };

    template<class _Dom>
    struct _RefFunClos<_Expr,_Dom> :
        _FunBase<_Dom, const typename _Dom::value_type&> {
        typedef _FunBase<_Dom, const typename _Dom::value_type&> _Base;
        typedef typename _Base::value_type value_type;
        typedef value_type _Tp;

        _RefFunClos (const _Dom& __e, _Tp __f (const _Tp&))
                : _Base (__e, __f) {}
    };

    template<typename _Tp>
    struct _RefFunClos<_ValArray,_Tp>
        : _FunBase<valarray<_Tp>, const _Tp&> {
        typedef _FunBase<valarray<_Tp>, const _Tp&> _Base;
        typedef _Tp value_type;
        
        _RefFunClos (const valarray<_Tp>& __v, _Tp __f(const _Tp&))
                : _Base (__e, __f) {}
    };
    
    //
    // Unary expression closure.
    //

    template<template<class> class _Oper, typename _Arg>
    class _UnBase {
    public:
        typedef _Oper<typename _Arg::value_type> _Op;
        typedef typename _Op::result_type value_type;

        _UnBase (const _Arg& __e) : _M_expr(__e) {}
        value_type operator[] (size_t) const;
        size_t size () const { return _M_expr.size (); }

    private:
        const _Arg& _M_expr;
    };

    template<template<class> class _Oper, typename _Arg>
    inline typename _UnBase<_Oper, _Arg>::value_type
    _UnBase<_Oper, _Arg>::operator[] (size_t __i) const
    { return _Op() (_M_expr[__i]); }
    
    template<template<class> class _Oper, class _Dom>
    struct _UnClos<_Oper, _Expr, _Dom> :  _UnBase<_Oper, _Dom> {
        typedef _Dom _Arg;
        typedef _UnBase<_Oper, _Dom> _Base;
        typedef typename _Base::value_type value_type;
        
        _UnClos (const _Arg& __e) : _Base(__e) {}
    };

    template<template<class> class _Oper, typename _Tp>
    struct _UnClos<_Oper, _ValArray, _Tp> : _UnBase<_Oper, valarray<_Tp> > {
        typedef valarray<_Tp> _Arg;
        typedef _UnBase<_Oper, valarray<_Tp> > _Base;
        typedef typename _Base::value_type value_type;

        _UnClos (const _Arg& __e) : _Base(__e) {}
    };


    //
    // Binary expression closure.
    //

    template<template<class> class _Oper,
        typename _FirstArg, typename _SecondArg>
    class _BinBase {
    public:
        typedef _Oper<typename _FirstArg::value_type> _Op;
        typedef typename _Op::result_type value_type;

        _BinBase (const _FirstArg& __e1, const _SecondArg& __e2)
                : _M_expr1 (__e1), _M_expr2 (__e2) {}
        value_type operator[] (size_t) const;
        size_t size () const { return _M_expr1.size (); }
        
    private:
        const _FirstArg& _M_expr1;
        const _SecondArg& _M_expr2;
    };

    template<template<class> class _Oper,
        typename _FirstArg, typename _SecondArg>
    inline typename _BinBase<_Oper,_FirstArg,_SecondArg>::value_type
    _BinBase<_Oper,_FirstArg,_SecondArg>::operator[] (size_t __i) const
    { return _Op() (_M_expr1[__i], _M_expr2[__i]); }


    template<template<class> class _Oper, class _Clos>
    class _BinBase2 {
    public:
        typedef typename _Clos::value_type _Vt;
        typedef _Oper<_Vt> _Op;
        typedef typename _Op::result_type value_type;

        _BinBase2 (const _Clos& __e, const _Vt& __t)
                : _M_expr1 (__e), _M_expr2 (__t) {}
        value_type operator[] (size_t) const;
        size_t size () const { return _M_expr1.size (); }

    private:
        const _Clos& _M_expr1;
        const _Vt& _M_expr2;
    };

    template<template<class> class _Oper, class _Clos>
    inline typename _BinBase2<_Oper,_Clos>::value_type
    _BinBase2<_Oper,_Clos>::operator[] (size_t __i) const
    { return _Op() (_M_expr1[__i], _M_expr2); }


    template<template<class> class _Oper, class _Clos>
    class _BinBase1 {
    public:
        typedef typename _Clos::value_type _Vt;
        typedef _Oper<_Vt> _Op;
        typedef typename _Op::result_type value_type;

        _BinBase1 (const _Vt& __t, const _Clos& __e)
                : _M_expr1 (__t), _M_expr2 (__e) {}
        value_type operator[] (size_t) const;
        size_t size () const { return _M_expr2.size (); }

    private:
        const _Vt& _M_expr1;
        const _Clos& _M_expr2;
    };

    template<template<class> class _Oper, class _Clos>
    inline typename
    _BinBase1<_Oper,_Clos>::value_type
    _BinBase1<_Oper,_Clos>:: operator[] (size_t __i) const
    { return _Op() (_M_expr1, _M_expr2[__i]); }

    
    template<template<class> class _Oper, class _Dom1, class _Dom2>
    struct  _BinClos<_Oper, _Expr, _Expr, _Dom1, _Dom2>
        : _BinBase<_Oper,_Dom1,_Dom2> {
        typedef _BinBase<_Oper,_Dom1,_Dom2> _Base;
        typedef typename _Base::value_type value_type;
        
        _BinClos(const _Dom1& __e1, const _Dom2& __e2) : _Base(__e1, __e2) {}
    };

    template<template<class> class _Oper, typename _Tp>
    struct _BinClos<_Oper,_ValArray,_ValArray,_Tp,_Tp>
        : _BinBase<_Oper,valarray<_Tp>,valarray<_Tp> > {
        typedef _BinBase<_Oper,valarray<_Tp>,valarray<_Tp> > _Base;
        typedef _Tp value_type;

        _BinClos (const valarray<_Tp>& __v, const valarray<_Tp>& __w)
                : _Base (__v, __w) {}
    };

    template<template<class> class _Oper, class _Dom>
    struct  _BinClos<_Oper,_Expr,_ValArray,_Dom,typename _Dom::value_type>
        : _BinBase<_Oper,_Dom,valarray<typename _Dom::value_type> > {
        typedef typename _Dom::value_type _Tp;
        typedef _BinBase<_Oper,_Dom,valarray<_Tp> > _Base;
        typedef typename _Base::value_type value_type;

        _BinClos(const _Dom& __e1, const valarray<_Tp>& __e2)
                : _Base (__e1, __e2) {}
    };

    template<template<class> class _Oper, class _Dom>
    struct  _BinClos<_Oper,_ValArray,_Expr,typename _Dom::value_type,_Dom>
        : _BinBase<_Oper,valarray<typename _Dom::value_type>,_Dom> {
        typedef typename _Dom::value_type _Tp;
        typedef _BinBase<_Oper,valarray<_Tp>,_Dom> _Base;
        typedef typename _Base::value_type value_type;

        _BinClos (const valarray<_Tp>& __e1, const _Dom& __e2)
                : _Base (__e1, __e2) {}
    };

    template<template<class> class _Oper, class _Dom>
    struct _BinClos<_Oper,_Expr,_Constant,_Dom,typename _Dom::value_type>
        : _BinBase2<_Oper,_Dom> {
        typedef typename _Dom::value_type _Tp;
        typedef _BinBase2<_Oper,_Dom> _Base;
        typedef typename _Base::value_type value_type;

        _BinClos (const _Dom& __e1, const _Tp& __e2) : _Base (__e1, __e2) {}
    };

    template<template<class> class _Oper, class _Dom>
    struct _BinClos<_Oper,_Constant,_Expr,typename _Dom::value_type,_Dom>
        : _BinBase1<_Oper,_Dom> {
        typedef typename _Dom::value_type _Tp;
        typedef _BinBase1<_Oper,_Dom> _Base;
        typedef typename _Base::value_type value_type;

        _BinClos (const _Tp& __e1, const _Dom& __e2) : _Base (__e1, __e2) {}
    };
    
    template<template<class> class _Oper, typename _Tp>
    struct _BinClos<_Oper,_ValArray,_Constant,_Tp,_Tp>
        : _BinBase2<_Oper,valarray<_Tp> > {
        typedef _BinBase2<_Oper,valarray<_Tp> > _Base;
        typedef typename _Base::value_type value_type;

        _BinClos (const valarray<_Tp>& __v, const _Tp& __t)
                : _Base (__v, __t) {}
    };

    template<template<class> class _Oper, typename _Tp>
    struct _BinClos<_Oper,_Constant,_ValArray,_Tp,_Tp>
        : _BinBase1<_Oper,valarray<_Tp> > {
        typedef _BinBase1<_Oper,valarray<_Tp> > _Base;
        typedef typename _Base::value_type value_type;

        _BinClos (const _Tp& __t, const valarray<_Tp>& __v)
                : _Base (__t, __v) {}
    };
        

    //
    // slice_array closure.
    //
    template<typename _Dom>  class _SBase {
    public:
        typedef typename _Dom::value_type value_type;

        _SBase (const _Dom& __e, const slice& __s)
                : _M_expr (__e), _M_slice (__s) {}
        value_type operator[] (size_t __i) const
        { return _M_expr[_M_slice.start () + __i * _M_slice.stride ()]; }
        size_t size() const { return _M_slice.size (); }

    private:
        const _Dom& _M_expr;
        const slice& _M_slice;
    };

    template<typename _Tp> class _SBase<_Array<_Tp> > {
    public:
        typedef _Tp value_type;

        _SBase (_Array<_Tp> __a, const slice& __s)
                : _M_array (__a._M_data+__s.start()), _M_size (__s.size()),
                  _M_stride (__s.stride()) {}
        value_type operator[] (size_t __i) const
        { return _M_array._M_data[__i * _M_stride]; }
        size_t size() const { return _M_size; }

    private:
        const _Array<_Tp> _M_array;
        const size_t _M_size;
        const size_t _M_stride;
    };

    template<class _Dom> struct  _SClos<_Expr,_Dom> : _SBase<_Dom> {
        typedef _SBase<_Dom> _Base;
        typedef typename _Base::value_type value_type;
        
        _SClos (const _Dom& __e, const slice& __s) : _Base (__e, __s) {}
    };

    template<typename _Tp>
    struct _SClos<_ValArray,_Tp> : _SBase<_Array<_Tp> > {
        typedef  _SBase<_Array<_Tp> > _Base;
        typedef _Tp value_type;

        _SClos (_Array<_Tp> __a, const slice& __s) : _Base (__a, __s) {}
    };

    //
    // gslice_array closure.
    //
    template<class _Dom> class _GBase {
    public:
        typedef typename _Dom::value_type value_type;
        
        _GBase (const _Dom& __e, const valarray<size_t>& __i)
                : _M_expr (__e), _M_index(__i) {}
        value_type operator[] (size_t __i) const
        { return _M_expr[_M_index[__i]]; }
        size_t size () const { return _M_index.size(); }
        
    private:
        const _Dom&	 _M_expr;
        const valarray<size_t>& _M_index;
    };
    
    template<typename _Tp> class _GBase<_Array<_Tp> > {
    public:
        typedef _Tp value_type;
        
        _GBase (_Array<_Tp> __a, const valarray<size_t>& __i)
                : _M_array (__a), _M_index(__i) {}
        value_type operator[] (size_t __i) const
        { return _M_array._M_data[_M_index[__i]]; }
        size_t size () const { return _M_index.size(); }
        
    private:
        const _Array<_Tp>     _M_array;
        const valarray<size_t>& _M_index;
    };

    template<class _Dom> struct _GClos<_Expr,_Dom> : _GBase<_Dom> {
        typedef _GBase<_Dom> _Base;
        typedef typename _Base::value_type value_type;

        _GClos (const _Dom& __e, const valarray<size_t>& __i)
                : _Base (__e, __i) {}
    };

    template<typename _Tp>
    struct _GClos<_ValArray,_Tp> : _GBase<_Array<_Tp> > {
        typedef _GBase<_Array<_Tp> > _Base;
        typedef typename _Base::value_type value_type;

        _GClos (_Array<_Tp> __a, const valarray<size_t>& __i)
                : _Base (__a, __i) {}
    };

    //
    // indirect_array closure
    //

    template<class _Dom> class _IBase {
    public:
        typedef typename _Dom::value_type value_type;

        _IBase (const _Dom& __e, const valarray<size_t>& __i)
                : _M_expr (__e), _M_index (__i) {}
        value_type operator[] (size_t __i) const
        { return _M_expr[_M_index[__i]]; }
        size_t size() const { return _M_index.size(); }
        
    private:
        const _Dom& 	    _M_expr;
        const valarray<size_t>& _M_index;
    };

    template<class _Dom> struct _IClos<_Expr,_Dom> : _IBase<_Dom> {
        typedef _IBase<_Dom> _Base;
        typedef typename _Base::value_type value_type;

        _IClos (const _Dom& __e, const valarray<size_t>& __i)
                : _Base (__e, __i) {}
    };

    template<typename _Tp>
    struct _IClos<_ValArray,_Tp>  : _IBase<valarray<_Tp> > {
        typedef _IBase<valarray<_Tp> > _Base;
        typedef _Tp value_type;

        _IClos (const valarray<_Tp>& __a, const valarray<size_t>& __i)
                : _Base (__a, __i) {}
    };

    //
    // class _Expr
    //      
    template<class _Clos, typename _Tp> class _Expr {
    public:
        typedef _Tp value_type;
        
        _Expr (const _Clos&);
        
        const _Clos& operator() () const;
        
        value_type operator[] (size_t) const;
        valarray<value_type> operator[] (slice) const;
        valarray<value_type> operator[] (const gslice&) const;
        valarray<value_type> operator[] (const valarray<bool>&) const;
        valarray<value_type> operator[] (const valarray<size_t>&) const;
    
        _Expr<_UnClos<_Unary_plus,_Expr,_Clos>, value_type>
        operator+ () const;

        _Expr<_UnClos<negate,_Expr,_Clos>, value_type>
        operator- () const;

        _Expr<_UnClos<_Bitwise_not,_Expr,_Clos>, value_type>
        operator~ () const;

        _Expr<_UnClos<logical_not,_Expr,_Clos>, bool>
        operator! () const;

        size_t size () const;
        value_type sum () const;
        
        valarray<value_type> shift (int) const;
        valarray<value_type> cshift (int) const;
//     _Meta<_ApplyFunctionWithValue<_Expr>, value_type>
//     apply (value_type _M_func (value_type)) const;
//     _Meta<_ApplyFunctionWithConstRef<_Expr>, value_type>
//     apply (value_type _M_func (const value_type&)) const;
        
    private:
        const _Clos _M_closure;
    };
    
    template<class _Clos, typename _Tp>
    inline
    _Expr<_Clos,_Tp>::_Expr (const _Clos& __c) : _M_closure(__c) {}
    
    template<class _Clos, typename _Tp>
    inline const _Clos&
    _Expr<_Clos,_Tp>::operator() () const
    { return _M_closure; }

    template<class _Clos, typename _Tp>
    inline _Tp
    _Expr<_Clos,_Tp>::operator[] (size_t __i) const
    { return _M_closure[__i]; }

    template<class _Clos, typename _Tp>
    inline valarray<_Tp>
    _Expr<_Clos,_Tp>::operator[] (slice __s) const
    { return _M_closure[__s]; }
    
    template<class _Clos, typename _Tp>
    inline valarray<_Tp>
    _Expr<_Clos,_Tp>::operator[] (const gslice& __gs) const
    { return _M_closure[__gs]; }
    
    template<class _Clos, typename _Tp>
    inline valarray<_Tp>
    _Expr<_Clos,_Tp>::operator[] (const valarray<bool>& __m) const
    { return _M_closure[__m]; }
    
    template<class _Clos, typename _Tp>
    inline valarray<_Tp>
    _Expr<_Clos,_Tp>::operator[] (const valarray<size_t>& __i) const
    { return _M_closure[__i]; }
    
    template<class _Clos, typename _Tp>
    inline size_t
    _Expr<_Clos,_Tp>::size () const  { return _M_closure.size (); }
    
    // XXX: replace this with a more robust summation algorithm.
    template<class _Clos, typename _Tp>
    inline _Tp
    _Expr<_Clos,_Tp>::sum () const
    {
        size_t __n = _M_closure.size();
        if (__n == 0) return _Tp();
        else {
            _Tp __s = _M_closure[--__n];
            while (__n != 0) __s += _M_closure[--__n];
            return __s;
        }
    }
    
    template<class _Dom, typename _Tp>
    inline _Tp
    min (const _Expr<_Dom,_Tp>& __e)
    {
        size_t __s = __e.size ();
        _Tp  __m = __e[0];
        for (size_t __i=1; __i<__s; ++__i)
            if (__m > __e[__i]) __m = __e[__i];
        return __m;
    }
    
    template<class _Dom, typename _Tp>
    inline _Tp
    max (const _Expr<_Dom,_Tp>& __e)
    {
        size_t __s = __e.size();
        _Tp __m = __e[0];
        for (size_t __i=1; __i<__s; ++__i)
            if (__m < __e[__i]) __m = __e[__i];
        return __m;
    }
    
    template<class _Dom, typename _Tp>
    inline _Expr<_UnClos<logical_not,_Expr,_Dom>, bool>
    _Expr<_Dom,_Tp>::operator! () const
    {
        typedef _UnClos<logical_not,_Expr,_Dom> _Closure;
        return _Expr<_Closure,_Tp> (_Closure(this->_M_closure));
    }

#define _DEFINE_EXPR_UNARY_OPERATOR(_Op, _Name)                         \
template<class _Dom, typename _Tp>                                      \
inline _Expr<_UnClos<_Name,_Expr,_Dom>,_Tp>                             \
_Expr<_Dom,_Tp>::operator##_Op () const                                 \
{                                                                       \
    typedef _UnClos<_Name,_Expr,_Dom> _Closure;                         \
    return _Expr<_Closure,_Tp> (_Closure (this->_M_closure));           \
}

    _DEFINE_EXPR_UNARY_OPERATOR(+, _Unary_plus)
    _DEFINE_EXPR_UNARY_OPERATOR(-, negate)
    _DEFINE_EXPR_UNARY_OPERATOR(~, _Bitwise_not)

#undef _DEFINE_EXPR_UNARY_OPERATOR


#define _DEFINE_EXPR_BINARY_OPERATOR(_Op, _Name)                        \
template<class _Dom1, class _Dom2>					\
inline _Expr<_BinClos<_Name,_Expr,_Expr,_Dom1,_Dom2>,                   \
             typename _Name<typename _Dom1::value_type>::result_type>   \
operator##_Op (const _Expr<_Dom1,typename _Dom1::value_type>& __v,      \
              const _Expr<_Dom2,typename _Dom2::value_type>& __w)       \
{                                                                       \
    typedef typename _Dom1::value_type _Arg;                            \
    typedef typename _Name<_Arg>::result_type _Value;                   \
    typedef _BinClos<_Name,_Expr,_Expr,_Dom1,_Dom2> _Closure;           \
    return _Expr<_Closure,_Value> (_Closure (__v (), __w ()));          \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinClos<_Name,_Expr,_Constant,_Dom,typename _Dom::value_type>, \
             typename _Name<typename _Dom::value_type>::result_type>    \
operator##_Op (const _Expr<_Dom,typename _Dom::value_type>& __v,        \
              const typename _Dom::value_type& __t)                     \
{                                                                       \
    typedef typename _Dom::value_type _Arg;                             \
    typedef typename _Name<_Arg>::result_type _Value;                   \
    typedef _BinClos<_Name,_Expr,_Constant,_Dom,_Arg> _Closure;         \
    return _Expr<_Closure,_Value> (_Closure (__v (), __t));             \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinClos<_Name,_Constant,_Expr,typename _Dom::value_type,_Dom>, \
             typename _Name<typename _Dom::value_type>::result_type>    \
operator##_Op (const typename _Dom::value_type& __t,                    \
               const _Expr<_Dom,typename _Dom::value_type>& __v)        \
{                                                                       \
    typedef typename _Dom::value_type _Arg;                             \
    typedef typename _Name<_Arg>::result_type _Value;                   \
    typedef _BinClos<_Name,_Constant,_Expr,_Arg,_Dom> _Closure;         \
    return _Expr<_Closure,_Value> (_Closure (__t, __v ()));             \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinClos<_Name,_Expr,_ValArray,_Dom,typename _Dom::value_type>, \
             typename _Name<typename _Dom::value_type>::result_type>    \
operator##_Op (const _Expr<_Dom,typename _Dom::value_type>& __e,        \
               const valarray<typename _Dom::value_type>& __v)          \
{                                                                       \
    typedef typename _Dom::value_type _Arg;                             \
    typedef typename _Name<_Arg>::result_type _Value;                   \
    typedef _BinClos<_Name,_Expr,_ValArray,_Dom,_Arg> _Closure;         \
    return  _Expr<_Closure,_Value> (_Closure (__e (), __v));            \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinClos<_Name,_ValArray,_Expr,typename _Dom::value_type,_Dom>, \
             typename _Name<typename _Dom::value_type>::result_type>    \
operator##_Op (const valarray<typename _Dom::value_type>& __v,          \
               const _Expr<_Dom,typename _Dom::value_type>& __e)        \
{                                                                       \
    typedef typename _Dom::value_type _Tp;                              \
    typedef typename _Name<_Tp>::result_type _Value;                    \
    typedef _BinClos<_Name,_ValArray,_Expr,_Tp,_Dom> _Closure;          \
    return _Expr<_Closure,_Value> (_Closure (__v, __e ()));             \
}

    _DEFINE_EXPR_BINARY_OPERATOR(+, plus)
    _DEFINE_EXPR_BINARY_OPERATOR(-, minus)
    _DEFINE_EXPR_BINARY_OPERATOR(*, multiplies)
    _DEFINE_EXPR_BINARY_OPERATOR(/, divides)
    _DEFINE_EXPR_BINARY_OPERATOR(%, modulus)
    _DEFINE_EXPR_BINARY_OPERATOR(^, _Bitwise_xor)
    _DEFINE_EXPR_BINARY_OPERATOR(&, _Bitwise_and)
    _DEFINE_EXPR_BINARY_OPERATOR(|, _Bitwise_or)
    _DEFINE_EXPR_BINARY_OPERATOR(<<, _Shift_left)
    _DEFINE_EXPR_BINARY_OPERATOR(>>, _Shift_right)

#undef _DEFINE_EXPR_BINARY_OPERATOR
    
#define _DEFINE_EXPR_RELATIONAL_OPERATOR(_Op, _Name)                    \
template<class _Dom1, class _Dom2>					\
inline _Expr<_BinClos<_Name,_Expr,_Expr,_Dom1,_Dom2>, bool>             \
operator##_Op (const _Expr<_Dom1,typename _Dom1::value_type>& __v,      \
              const _Expr<_Dom2,typename _Dom2::value_type>& __w)       \
{                                                                       \
    typedef typename _Dom1::value_type _Arg;                            \
    typedef _BinClos<_Name,_Expr,_Expr,_Dom1,_Dom2> _Closure;           \
    return _Expr<_Closure,bool> (_Closure (__v (), __w ()));            \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinClos<_Name,_Expr,_Constant,_Dom,typename _Dom::value_type>, \
             bool>                                                      \
operator##_Op (const _Expr<_Dom,typename _Dom::value_type>& __v,        \
              const typename _Dom::value_type& __t)                     \
{                                                                       \
    typedef typename _Dom::value_type _Arg;                             \
    typedef _BinClos<_Name,_Expr,_Constant,_Dom,_Arg> _Closure;         \
    return _Expr<_Closure,bool> (_Closure (__v (), __t));               \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinClos<_Name,_Constant,_Expr,typename _Dom::value_type,_Dom>, \
             bool>                                                      \
operator##_Op (const typename _Dom::value_type& __t,                    \
               const _Expr<_Dom,typename _Dom::value_type>& __v)        \
{                                                                       \
    typedef typename _Dom::value_type _Arg;                             \
    typedef _BinClos<_Name,_Constant,_Expr,_Arg,_Dom> _Closure;         \
    return _Expr<_Closure,bool> (_Closure (__t, __v ()));               \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinClos<_Name,_Expr,_ValArray,_Dom,typename _Dom::value_type>, \
             bool>                                                      \
operator##_Op (const _Expr<_Dom,typename _Dom::value_type>& __e,        \
               const valarray<typename _Dom::value_type>& __v)          \
{                                                                       \
    typedef typename _Dom::value_type _Tp;                              \
    typedef _BinClos<_Name,_Expr,_ValArray,_Dom,_Tp> _Closure;          \
    return  _Expr<_Closure,bool> (_Closure (__e (), __v));              \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinClos<_Name,_ValArray,_Expr,typename _Dom::value_type,_Dom>, \
             bool>                                                      \
operator##_Op (const valarray<typename _Dom::value_type>& __v,          \
               const _Expr<_Dom,typename _Dom::value_type>& __e)        \
{                                                                       \
    typedef typename _Dom::value_type _Tp;                              \
    typedef _BinClos<_Name,_ValArray,_Expr,_Tp,_Dom> _Closure;          \
    return _Expr<_Closure,bool> (_Closure (__v, __e ()));               \
}

    _DEFINE_EXPR_RELATIONAL_OPERATOR(&&, logical_and)
    _DEFINE_EXPR_RELATIONAL_OPERATOR(||, logical_or)
    _DEFINE_EXPR_RELATIONAL_OPERATOR(==, equal_to)
    _DEFINE_EXPR_RELATIONAL_OPERATOR(!=, not_equal_to)
    _DEFINE_EXPR_RELATIONAL_OPERATOR(<, less)
    _DEFINE_EXPR_RELATIONAL_OPERATOR(>, greater)
    _DEFINE_EXPR_RELATIONAL_OPERATOR(<=, less_equal)
    _DEFINE_EXPR_RELATIONAL_OPERATOR(>=, greater_equal)

#undef _DEFINE_EXPR_RELATIONAL_OPERATOR



#define _DEFINE_EXPR_UNARY_FUNCTION(_Name)                              \
template<class _Dom>                                                    \
inline _Expr<_UnFunClos<_Expr,_Dom>,typename _Dom::value_type>          \
_Name(const _Expr<_Dom,typename _Dom::value_type>& __e)                 \
{                                                                       \
    typedef typename _Dom::value_type _Tp;                              \
    typedef _UnFunClos<_Expr,_Dom> _Closure;                            \
    return _Expr<_Closure,_Tp>(_Closure(__e(), (_Tp(*)(_Tp))(&_Name))); \
}                                                                       \
                                                                        \
template<typename _Tp>                                                  \
inline _Expr<_UnFunClos<_ValArray,_Tp>,_Tp>                             \
_Name(const valarray<_Tp>& __v)                                         \
{                                                                       \
    typedef _UnFunClos<_ValArray,_Tp> _Closure;                         \
    return _Expr<_Closure,_Tp> (_Closure (__v, (_Tp(*)(_Tp))(&_Name))); \
}


    _DEFINE_EXPR_UNARY_FUNCTION(abs)
    _DEFINE_EXPR_UNARY_FUNCTION(cos)
    _DEFINE_EXPR_UNARY_FUNCTION(acos)
    _DEFINE_EXPR_UNARY_FUNCTION(cosh)    
    _DEFINE_EXPR_UNARY_FUNCTION(sin)
    _DEFINE_EXPR_UNARY_FUNCTION(asin)
    _DEFINE_EXPR_UNARY_FUNCTION(sinh)    
    _DEFINE_EXPR_UNARY_FUNCTION(tan)
    _DEFINE_EXPR_UNARY_FUNCTION(tanh)
    _DEFINE_EXPR_UNARY_FUNCTION(atan)
    _DEFINE_EXPR_UNARY_FUNCTION(exp)    
    _DEFINE_EXPR_UNARY_FUNCTION(log)
    _DEFINE_EXPR_UNARY_FUNCTION(log10)
    _DEFINE_EXPR_UNARY_FUNCTION(sqrt)

#undef _DEFINE_EXPR_UNARY_FUNCTION


#define _DEFINE_EXPR_BINARY_FUNCTION(_Name)                             \
template<class _Dom1, class _Dom2>                                      \
inline _Expr<_BinFunClos<_Expr,_Expr,_Dom1,_Dom2>,typename _Dom1::value_type>\
_Name (const _Expr<_Dom1,typename _Dom1::value_type>& __e1,             \
       const _Expr<_Dom2,typename _Dom2::value_type>& __e2)             \
{                                                                       \
    typedef typename _Dom1::value_type _Tp;                             \
    typedef _BinFunClos<_Expr,_Expr,_Dom1,_Dom2> _Closure;              \
    return _Expr<_Closure,_Tp>                                          \
        (_Closure (__e1 (), __e2 (), (_Tp(*)(_Tp, _Tp))(&_Name)));      \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinFunClos<_Expr,_ValArray,_Dom,typename _Dom::value_type>, \
             typename _Dom::value_type>                                 \
_Name (const _Expr<_Dom,typename _Dom::value_type>& __e,                \
       const valarray<typename _Dom::value_type>& __v)                  \
{                                                                       \
    typedef typename _Dom::value_type _Tp;                              \
    typedef _BinFunClos<_Expr,_ValArray,_Dom,_Tp> _Closure;             \
    return _Expr<_Closure,_Tp>                                          \
        (_Closure (__e (), __v, (_Tp(*)(_Tp, _Tp))(&_Name)));           \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinFunClos<_ValArray,_Expr,typename _Dom::value_type,_Dom>, \
             typename _Dom::value_type>                                 \
_Name (const valarray<typename _Dom::valarray>& __v,                    \
       const _Expr<_Dom,typename _Dom::value_type>& __e)                \
{                                                                       \
    typedef typename _Dom::value_type _Tp;                              \
    typedef _BinFunClos<_ValArray,_Expr,_Tp,_Dom> _Closure;             \
    return _Expr<_Closure,_Tp>                                          \
        (_Closure (__v, __e (), (_Tp(*)(_Tp, _Tp))(&_Name)));           \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinFunClos<_Expr,_Constant,_Dom,typename _Dom::value_type>, \
             typename _Dom::value_type>                                 \
_Name (const _Expr<_Dom, typename _Dom::value_type>& __e,               \
       const typename _Dom::value_type& __t)                            \
{                                                                       \
    typedef typename _Dom::value_type _Tp;                              \
    typedef _BinFunClos<_Expr,_Constant,_Dom,_Tp> _Closure;             \
    return _Expr<_Closure,_Tp>                                          \
        (_Closure (__e (), __t, (_Tp(*)(_Tp, _Tp))(&_Name)));           \
}                                                                       \
                                                                        \
template<class _Dom>                                                    \
inline _Expr<_BinFunClos<_Constant,_Expr,typename _Dom::value_type,_Dom>, \
             typename _Dom::value_type>                                 \
_Name (const typename _Dom::value_type& __t,                            \
       const _Expr<_Dom,typename _Dom::value_type>& __e)                \
{                                                                       \
    typedef typename _Dom::value_type _Tp;                              \
    typedef _BinFunClos<_Constant,_Expr,_Tp,_Dom> _Closure;             \
    return _Expr<_Closure,_Tp>                                          \
        (_Closure (__t, __e (), (_Tp(*)(_Tp, _Tp))(&_Name)));           \
}                                                                       \
                                                                        \
template<typename _Tp>                                                  \
inline _Expr<_BinFunClos<_ValArray,_ValArray,_Tp,_Tp>, _Tp>             \
_Name (const valarray<_Tp>& __v, const valarray<_Tp>& __w)              \
{                                                                       \
    typedef _BinFunClos<_ValArray,_ValArray,_Tp,_Tp> _Closure;          \
    return _Expr<_Closure,_Tp>                                          \
        (_Closure (__v, __w, (_Tp(*)(_Tp,_Tp))(&_Name)));               \
}                                                                       \
                                                                        \
template<typename _Tp>                                                  \
inline _Expr<_BinFunClos<_ValArray,_Constant,_Tp,_Tp>,_Tp>              \
_Name (const valarray<_Tp>& __v, const _Tp& __t)                        \
{                                                                       \
    typedef _BinFunClos<_ValArray,_Constant,_Tp,_Tp> _Closure;          \
    return _Expr<_Closure,_Tp>                                          \
        (_Closure (__v, __t, (_Tp(*)(_Tp,_Tp))(&_Name)));               \
}                                                                       \
                                                                        \
template<typename _Tp>                                                  \
inline _Expr<_BinFunClos<_Constant,_ValArray,_Tp,_Tp>,_Tp>              \
_Name (const _Tp& __t, const valarray<_Tp>& __v)                        \
{                                                                       \
    typedef _BinFunClos<_Constant,_ValArray,_Tp,_Tp> _Closure;          \
    return _Expr<_Closure,_Tp>                                          \
        (_Closure (__t, __v, (_Tp(*)(_Tp,_Tp))(&_Name)));               \
}

_DEFINE_EXPR_BINARY_FUNCTION(atan2)
_DEFINE_EXPR_BINARY_FUNCTION(pow)

#undef _DEFINE_EXPR_BINARY_FUNCTION

} // std::


#endif /* _CPP_VALARRAY_META_H */

// Local Variables:
// mode:c++
// End:
