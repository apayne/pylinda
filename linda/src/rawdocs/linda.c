/**\brief The Linda Package

\code
import linda
linda.connect() # connect must be called first
\endcode

\author Andrew Wilkinson <aw@cs.york.ac.uk>
\version 0.4
*/
namespace linda {

/** \class TupleSpace
  * \brief A class representing a TupleSpace
  *
  * Creating an instance of this class creates a new tuplespace in the linda network.
  */
class TupleSpace {
public:
    /** \brief The TupleSpace constructor
      *
      * Creates a new tuplespace, to give other processes access to this tuplespace output the reference
      * in a tuple.
      */
    TupleSpace();

    /** \brief The TupleSpace destructor.
      *
      * Deletes this reference to the tuplespace.
      */
    ~TupleSpace();

    /** \brief Output a tuple to the tuplespace.
      * \param t The Tuple to be outputted.
      *
      * \code
      * ts._out((1, 2, 3))
      * \endcode
      *
      * \attention Only ints, floats, booleans, strings, tuples and tuplespace references may be part of the tuple.
      * For performance reasons this is not checked and may cause a strange run-time error if not
      * followed.
      */
    void _out(Tuple t);

    /** \brief Destructively read a tuple from the tuplespace
      * \param t A template which is matched against the tuples in the tuplespace.
      * \return The matching tuple.
      *
      * This function call blocks until a tuple matching the template is placed in the tuplespace
      *
      * \code
      * t = ts._in((1, 2, 3))
      * t = ts._in((int, 2, 3))
      * t = ts._in((int, int, 3))
      * t = ts._in((int, int, int))
      * \endcode
      *
      * The template should be a tuple containing either actual parameters, or formals. An actual paramater is a
      * real value like 1, "abc" or 1.0. A formal parameter is a type which matched any value of that type. For
      * example <i>int</i> will match 1, 2, 3 etc. linda.TupleSpace will match any tuplespace reference.
      */
    Tuple _in(Template t);

    /** \brief Non-destructively read a tuple from the tuplespace
      * \param t A template which is matched against the tuples in the tuplespace.
      * \return The matching tuple.
      *
      * This function call blocks until a tuple matching the template is placed in the tuplespace
      *
      * \code
      * t = ts._rd((int, int, int))
      * \endcode
      *
      * See linda::TupleSpace::_in for details on the format of the template
      */
    Tuple _rd(Template t);

    /** \brief Destructively read a tuple from the tuplespace
      * \param t A template which is matched against the tuples in the tuplespace.
      * \return The matching tuple or None if a deadlock is detected.
      *
      * This function call blocks until a tuple matching the template is placed in the tuplespace or a deadlock
      * situation is detected as described in the paper "A Principled Semantics for inp" by Jacob and Wood [2000].
      * If a deadlock is detected this function returns None.
      *
      * \code
      * t = ts._inp((int, int, int))
      * \endcode
      *
      * See linda::TupleSpace::_in for details on the format of the template
      */
    Tuple _inp(Template t);

    /** \brief Non-destructively read a tuple from the tuplespace
      * \param t A template which is matched against the tuples in the tuplespace
      * \return The matching tuple or None if a deadlock is detected
      *
      * This function call blocks until a tuple matching the template is placed in the tuplespace or a deadlock
      * situation is detected as described in the paper "A Principled Semantics for inp" by Jacob and Wood [2000].
      * If a deadlock is detected this function returns None.
      *
      * \code
      * t = ts._rdp((int, int, int))
      * \endcode
      *
      * See linda::TupleSpace::_in for details on the format of the template
      */
    Tuple _rdp(Template t);

    /** \brief Move all tuples matching the template to another tuplespace
      * \param dest The tuplespace to which the tuples are moved
      * \param t A template which is matched against the tuples in the tuplespace
      * \return The number of tuples moved.
      *
      * \code
      * count = ts1.collect(ts2, (int, int, int))
      * \endcode
      *
      * See linda::TupleSpace::_in for details on the format of the template
      */
    int collect(TupleSpace dest, Template t);


    /** \brief Copy all tuples matching the template to another tuplespace
      * \param dest The tuplespace to which the tuples are moved
      * \param t A template which is matched against the tuples in the tuplespace
      * \return The number of tuples copied.
      *
      * \code
      * count = ts1.copy_collect(ts2, (int, int, int))
      * \endcode
      *
      * See linda::TupleSpace::_in for details on the format of the template
      */
    int copy_collect(TupleSpace dest, Template t);
};

/** The universal tuplespace.
  * All processes in the linda network have access to this tuplespace.
  */
TupleSpace universe;

/** Connect to a Linda server running on the local computer.
  */
void connect();


}
