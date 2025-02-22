// -*- c++ -*-
// Generated by gtkmmproc -- DO NOT MODIFY!
#ifndef _GSTREAMERMM_ITERATOR_H
#define _GSTREAMERMM_ITERATOR_H


#include <glibmm.h>

/* gstreamermm - a C++ wrapper for gstreamer
 *
 * Copyright 2008 The gstreamermm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <gst/gstiterator.h>
#include <gstreamermm/handle_error.h>


namespace Gst
{

/** @addtogroup gstreamermmEnums gstreamermm Enums and Flags */

/**
 * @ingroup gstreamermmEnums
 */
enum IteratorItem
{
  ITERATOR_ITEM_SKIP,
  ITERATOR_ITEM_PASS,
  ITERATOR_ITEM_END
};

} // namespace Gst


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Gst::IteratorItem> : public Glib::Value_Enum<Gst::IteratorItem>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{

/**
 * @ingroup gstreamermmEnums
 */
enum IteratorResult
{
  ITERATOR_DONE,
  ITERATOR_OK,
  ITERATOR_RESYNC,
  ITERATOR_ERROR
};

} // namespace Gst


#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Glib
{

template <>
class Value<Gst::IteratorResult> : public Glib::Value_Enum<Gst::IteratorResult>
{
public:
  static GType value_type() G_GNUC_CONST;
};

} // namespace Glib
#endif /* DOXYGEN_SHOULD_SKIP_THIS */


namespace Gst
{


/**  The base class for classes that retrieve multiple elements in a thread
 * safe way.
 *
 * Classes derived from Gst::IteratorBase are used to retrieve multiple objects
 * from another object in a thread safe way.  They are implemented as C++ like
 * input iterators so they do not support multi-passing, but they are usable
 * enough for iterating through a list of items and referencing them in a
 * single pass.
 *
 * Various GStreamer objects provide access to their internal structures using
 * an iterator.
 */
template <class CppType>
class IteratorBase
{
public:
  /** Moves to the next iterator item.
   *
   * @return The result of the iteration. MT safe. 
   */
  virtual IteratorResult next();

  /** Resynchronize the iterator. This function is mostly called after next()
   * returns Gst::ITERATOR_RESYNC.  A result of Gst::ITERATOR_RESYNC from
   * next() means that a concurrent update was made to the iterator list during
   * iteration and the iterator needs to be resynchronized before continuing.
   * Use this function to resynchronize.
   */
  void resync();

  /** Tells if the iterator is at the start of the list (not on the first item,
   * but just before it).  Increment the iterator or use
   * Gst::IteratorBasic::begin() to go to the first item.
   *
   * @return true if the iterator is at the start of the list, false otherwise.
   */
  bool is_start() const;

  /** Tells if the iterator is at the end of the list (just after the last
   * element).
   *
   * @return true if the iterator is at the end of the list, false otherwise.
   */
  bool is_end() const;

  /** Tells whether the iterator is valid and can be dereferenced.
   */
  operator bool() const;

  ///Provides access to the underlying C GObject.
  GstIterator*          cobj()          { return cobject_; };

  ///Provides access to the underlying C GObject.
  const GstIterator*    cobj() const    { return cobject_; };

  /** Frees the underlying C instance if a take_ownership value of true was
   * used to wrap it.
   */
  virtual ~IteratorBase();

protected:
  /// Default constructor.
  IteratorBase();

  /** Copy constructor.  Please note that copying and assigning merely shares
   * the underlying C object.  Operations on the copy are also performed in the
   * underlying C object of the original and if the original is destroyed, the
   * copy is invalid.
   */
  IteratorBase(const IteratorBase<CppType>&);

  /** Constructs an IteratorBase from an underlying C object.
   * @param castitem The underlying C object.
   * @param take_ownership Whether to take over the underlying C object.  If
   * true, C object is freed when wrapper is destroyed.
   */
  explicit IteratorBase(GstIterator* castitem, bool take_ownership = true);

  /** Assignment operator.  It replaces the contents of the iterator with the
   * contents of the new one freeing the underlying C object if a
   * take_ownership value of true was used when wrapping it.  Please note that
   * copying and assigning merely shares the underlying C object.  Operations
   * on the copy are also performed in the underlying C object of the original
   * and if the original is destroyed, the copy is invalid.
   */
  IteratorBase<CppType>& operator=(const IteratorBase<CppType>& other);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  void* current;         // The current element the iterator is referencing.
  IteratorResult current_result; // The current result of a next() call.
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  GstIterator* cobject_;    // The underlying  C object.
  bool take_ownership;      // Whether to destroy C object with the wrapper.
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

private:
  void swap(IteratorBase<CppType>& other);
};

/**  A class used to retrieve multiple elements in a thread safe way.
 * Gst::IteratorBasic iterates specifically through elements that are not
 * reference counted.  Gst::Iterator is used for iterating through reference
 * counted objects.
 */
template <class CppType>
class IteratorBasic : public IteratorBase<CppType>
{
public:
  ///Default constructor.
  IteratorBasic();

  /** Creates a Gst::IteratorBasic wrapper for a GstIterator object.  The
   * underlying @a castitem will be freed with the Gst::IteratorBasic
   * destruction if a take_ownership value of true is given.
   *
   * @param castitem The C instance to wrap.
   * @param take_ownership Whether to take over the underlying C object.  If
   * true, C object is freed when wrapper is destroyed.
   */
  explicit IteratorBasic(GstIterator* castitem, bool take_ownership = true);

  /** Resynchronizes the iterator and moves the iterator to the first item.
   *
   * @throw std::runtime_error (if a Gst::ITERATOR_ERROR is encountered or if a
   * concurrent update to the iterator occurs while it is advanced to the first
   * element).
   */
  void begin();

  /** Dereferences the iterator and obtains the underlying object.
   */
  CppType operator*() const;

  /** Accesses underlying object member through the iterator.
   */
  CppType* operator->() const;

  /** Prefix auto-increment operator.  It advances to the next item in the
   * iterator.  It is faster than the postfix operator.
   * @throw std::runtime_error (if a Gst::ITERATOR_ERROR is encountered or if a
   * concurrent update to the iterator occurs while it iterates).
   */
  IteratorBasic<CppType>& operator++();

  /** Postfix auto-increment operator.  It advances to the next item in the
   * iterator.
   * @throw std::runtime_error (if a Gst::ITERATOR_ERROR is encountered or if a
   * concurrent update to the iterator occurs while it iterates).
   */
  IteratorBasic<CppType> operator++(int);
};

/**  A class used to retrieve multiple reference counted elements in a thread
 * safe way.
 * Gst::Iterator iterates specifically through elements that are reference
 * counted and therefore dereferencing the elements of the iterator yields a
 * Glib::RefPtr<> to the C++ element type.
 */
template <class CppType>
class Iterator : public IteratorBasic<CppType>
{
public:
  ///Default constructor.
  Iterator();

  /** Creates a Gst::Iterator wrapper for a GstIterator object.  The underlying
   * @a castitem will be freed with the Gst::Iterator destruction if a
   * take_ownership value of true is given.
   *
   * @param castitem The C instance to wrap.
   * @param take_ownership Whether to take over the underlying C object.  If
   * true, C object is freed when wrapper is destroyed.
   */
  explicit Iterator(GstIterator* castitem, bool take_ownership = true);

  /** Moves to the next iterator item.
   *
   * @return The result of the iteration. MT safe. 
   */
  IteratorResult next();

  /** Dereferences the iterator and obtains the underlying Glib::RefPtr<>.
   */
  Glib::RefPtr<CppType> operator*() const;

  /** Accesses underlying object member through the RefPtr<>.
   */
  CppType* operator->() const;

  /** Prefix auto-increment operator.  It advances to the next item in the
   * iterator.  It is faster than the postfix operator.
   * @throw std::runtime_error (if a Gst::ITERATOR_ERROR is encountered or if a
   * concurrent update to the iterator occurs while it iterates).
   */
  Iterator<CppType>& operator++();

  /** Postfix auto-increment operator.  It advances to the next item in the
   * iterator.
   * @throw std::runtime_error (if a Gst::ITERATOR_ERROR is encountered or if a
   * concurrent update to the iterator occurs while it iterates).
   */
  Iterator<CppType> operator++(int);
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/***************** Gst::IteratorBase<CppType> ************************/

template<class CppType>
IteratorBase<CppType>::IteratorBase()
: current(0),
  current_result(Gst::ITERATOR_OK),
  cobject_(0),
  take_ownership(true)
{}

template<class CppType>
IteratorBase<CppType>::IteratorBase(const IteratorBase<CppType>& other)
  : current(other.current),
    current_result(other.current_result),
    cobject_(const_cast<GstIterator*>(other.cobj())),
    take_ownership((other.cobj()) ? false : true)
{}

template<class CppType>
IteratorBase<CppType>::IteratorBase(GstIterator* castitem, bool take_ownership)
: current(0),
  current_result(Gst::ITERATOR_OK),
  cobject_(castitem),
  take_ownership(take_ownership)
{}

template<class CppType>
IteratorBase<CppType>& IteratorBase<CppType>::operator=(const IteratorBase<CppType>& other)
{
  IteratorBase temp(other);
  swap(temp);
  return *this;
}

template<class CppType>
IteratorResult IteratorBase<CppType>::next()
{
  current_result =
    static_cast<Gst::IteratorResult>(gst_iterator_next(cobj(), &current));

  // Set current to null if iterator is done:
  if(current_result == Gst::ITERATOR_DONE)
    current = 0;

  return current_result;
}

template<class CppType>
void IteratorBase<CppType>::resync()
{
  gst_iterator_resync(cobj());
  current = 0;
  current_result = Gst::ITERATOR_OK;
}

template<class CppType>
bool IteratorBase<CppType>::is_start() const
{
  return (current == 0 && current_result == Gst::ITERATOR_OK);
}

template<class CppType>
bool IteratorBase<CppType>::is_end() const
{
  return (current_result == Gst::ITERATOR_DONE);
}

template<class CppType>
IteratorBase<CppType>::operator bool() const
{
  return (current != 0);
}

template<class CppType>
void IteratorBase<CppType>::swap(IteratorBase<CppType>& other)
{
  GstIterator *const temp_obj = cobject_;
  cobject_ = other.cobject_;
  other.cobject_ = temp_obj;

  const bool temp_take_ownership = take_ownership;
  take_ownership = other.take_ownership;
  other.take_ownership = temp_take_ownership;

  void* const temp_current = current;
  current = other.current;
  other.current = temp_current;

  const IteratorResult temp_result = current_result;
  current_result = other.current_result;
  other.current_result = temp_result;
}

//virtual
template<class CppType>
IteratorBase<CppType>::~IteratorBase()
{
  if(take_ownership && cobject_)
  {
    gst_iterator_free(cobject_);
    cobject_ = 0;
  }
}

/***************** Gst::IteratorBasic<CppType> ***********************/

template <class CppType>
IteratorBasic<CppType>::IteratorBasic()
  : IteratorBase<CppType>()
{}

template <class CppType>
IteratorBasic<CppType>::IteratorBasic(GstIterator* castitem,
  bool take_ownership)
  : IteratorBase<CppType>(castitem, take_ownership)
{}

template<class CppType>
void IteratorBasic<CppType>::begin()
{
  this->resync();
  ++(*this);
}

template <class CppType>
CppType IteratorBasic<CppType>::operator*() const
{
  typedef typename CppType::BaseObjectType CType;

  if(this->current)
    return CppType(static_cast<CType*>(this->current));
  else
    return CppType();
}

template <class CppType>
CppType* IteratorBasic<CppType>::operator->() const
{
  static typename CppType::CppObjectType result;

  if(this->current)
  {
    result = this->operator*();
    return &result;
  }
  else
    return static_cast<CppType*>(0);
}

template<class CppType>
IteratorBasic<CppType>& IteratorBasic<CppType>::operator++()
{
  const IteratorResult result = this->next();

  if(result == Gst::ITERATOR_RESYNC)
    gstreamermm_handle_error("Concurrent update of iterator elements.  "
      "Please resync.");
  else if(result == Gst::ITERATOR_ERROR)
    gstreamermm_handle_error("Iterator error while incrementing.");

  return *this;
}

template<class CppType>
IteratorBasic<CppType> IteratorBasic<CppType>::operator++(int)
{
  IteratorBasic<CppType> original = *this;
  ++(*this);
  return original;
}

/******************* Gst::Iterator<CppType> **************************/

template <class CppType>
Iterator<CppType>::Iterator()
  : IteratorBasic<CppType>()
{}

template <class CppType>
Iterator<CppType>::Iterator(GstIterator* castitem, bool take_ownership)
  : IteratorBasic<CppType>(castitem, take_ownership)
{}

template <class CppType>
IteratorResult Iterator<CppType>::next()
{
  const IteratorResult result = IteratorBasic<CppType>::next();

  // Remove extra reference that gst_iterator_next() takes because references
  // are taken when using Gst::Iterator<CppType> dereferencing operators (*
  // and ->).
  if(this->current)
    g_object_unref(this->current);

  return result;
}

template <class CppType>
Glib::RefPtr<CppType> Iterator<CppType>::operator*() const
{
  typedef typename CppType::BaseObjectType CType;

  if(this->current)
  {
    //Take extra reference when dereferencing.  The reference will disappear
    //when Glib::RefPtr<> is destroyed.
    return Glib::wrap(static_cast<CType*>(this->current), true);
  }
  else
    return Glib::RefPtr<CppType>(0);
}

template <class CppType>
CppType* Iterator<CppType>::operator->() const
{
  typedef typename CppType::BaseObjectType CType;

  if(this->current)
  {
    //Take extra reference when dereferencing.  The reference will disappear
    //when Glib::RefPtr<> is destroyed.
    return Glib::wrap(static_cast<CType*>(this->current), true).operator->();
  }
  else
    return static_cast<CppType*>(0);
}

template<class CppType>
Iterator<CppType>& Iterator<CppType>::operator++()
{
  IteratorBasic<CppType>::operator++();
  return *this;
}

template<class CppType>
Iterator<CppType> Iterator<CppType>::operator++(int)
{
  Iterator<CppType> original = *this;
  ++(*this);
  return original;
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

} //namespace Gst


#endif /* _GSTREAMERMM_ITERATOR_H */

