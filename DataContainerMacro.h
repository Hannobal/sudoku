#ifndef UTILITY_DATACONTAINERMACRO_H_
#define UTILITY_DATACONTAINERMACRO_H_

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/first_n.hpp>
#include <boost/preprocessor/seq/rest_n.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#include <boost/serialization/nvp.hpp>
/*

The DATA_CONTAINER macro creates a class with the following features:
- serializable via boost::serialize
- private member variables with public getters and setters
- default constructor with defined default values
- copy constructor (implicit)
- additional declarations can be added in the additionalDecl argument

syntax for dataMembers: ((name, storageType, defaultValue, IOType))
name:         the name of the member (for getter, setter and constructor)
              member variable will be m_<name>
storageType:  the type for the member variable
defaultValue: the initialization value set in the default constructor
IOType:       used as input type for the explicit constructor and the setter
              and return type of the getter

syntax for additionalDecl: (declaration)
the declaration string is simply copied into the public section

syntax for bases (DATA_CONTAINER_DERIVED) only:
((name, type, qualifiers))
name:       name for the constructor argument
type:       the class to inherit from
qualifiers: usually "public", if applicable also "virtual"

EXAMPLES ######################################################################

DATA_CONTAINER(ShapeSettings,                              class name
		((hasborder,  bool,   true, bool))                 data member
		((fillColor,  Color,  Color::White, Color const&)) data member *NOTE:* comma closes data member section
		(virtual shared_ptr<Shape> create() = 0;)          additional member function declaration
)

DATA_CONTAINER_DERIVED(RectangleSettings,           class name
		((shapeSettings, ShapeSettings, public))    base class to inherit from
		((otherBase, MySettings, public)),          another base class
		((width,      double, 1.0, double))         data member
		((height,     double, 1.0, double))),       data member *NOTE:* comma closes data member section
		(static RectangleSettings hugeRectangle;)   additional member variable declaration
		(shared_ptr<Shape> create() override;)      additional member function declaration
)
*/

#define DATA_CONTAINER(classname,dataMembers,additionalDecl) \
	class classname { \
	public: \
	\
	classname(\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ARG, , BOOST_PP_SEQ_FIRST_N(1,dataMembers))\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ARG_COMMA, , BOOST_PP_SEQ_REST_N(1,dataMembers))\
	) :\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ASSIGN, , BOOST_PP_SEQ_FIRST_N(1,dataMembers))\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ASSIGN_COMMA, , BOOST_PP_SEQ_REST_N(1,dataMembers))\
	{}\
	\
	classname() :\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_DEFAULT, , BOOST_PP_SEQ_FIRST_N(1,dataMembers))\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_DEFAULT_COMMA, , BOOST_PP_SEQ_REST_N(1,dataMembers))\
	{}\
	\
	BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_GETTER, , dataMembers) \
	\
	BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_SETTER, , dataMembers) \
	\
	BOOST_PP_SEQ_FOR_EACH(ADDITIONAL_DECLARATION, , additionalDecl) \
	\
	private: \
	\
	BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_DECLARATION, , dataMembers) \
	\
	friend class boost::serialization::access; \
	template<class Archive> \
	void serialize(Archive & ar, const unsigned int /* file_version */) { \
		ar BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_SERIALIZATION, , dataMembers); \
	} \
	};

#define DATA_CONTAINER_DERIVED(classname,bases,dataMembers,additionalDecl) \
	class classname : \
			BOOST_PP_SEQ_FOR_EACH(BASE_INHERITANCE, , BOOST_PP_SEQ_FIRST_N(1,bases))\
			BOOST_PP_SEQ_FOR_EACH(BASE_INHERITANCE_COMMA, , BOOST_PP_SEQ_REST_N(1,bases))\
	{ \
	public: \
	\
	classname(\
			BOOST_PP_SEQ_FOR_EACH(BASE_CONSTR_ARG, , BOOST_PP_SEQ_FIRST_N(1,bases))\
			BOOST_PP_SEQ_FOR_EACH(BASE_CONSTR_ARG_COMMA, , BOOST_PP_SEQ_REST_N(1,bases))\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ARG_COMMA, , dataMembers)\
	) :\
			BOOST_PP_SEQ_FOR_EACH(BASE_CONSTR_ASSIGN, , BOOST_PP_SEQ_FIRST_N(1,bases))\
			BOOST_PP_SEQ_FOR_EACH(BASE_CONSTR_ASSIGN_COMMA, , BOOST_PP_SEQ_REST_N(1,bases))\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ASSIGN_COMMA, , dataMembers)\
	{}\
	\
	classname() :\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_DEFAULT, , BOOST_PP_SEQ_FIRST_N(1,dataMembers))\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_DEFAULT_COMMA, , BOOST_PP_SEQ_REST_N(1,dataMembers))\
	{}\
	\
	BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_GETTER, , dataMembers) \
	\
	BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_SETTER, , dataMembers) \
	\
	BOOST_PP_SEQ_FOR_EACH(ADDITIONAL_DECLARATION, , additionalDecl) \
	\
	private: \
	\
	BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_DECLARATION, , dataMembers) \
	\
	friend class boost::serialization::access; \
	template<class Archive> \
	void serialize(Archive & ar, const unsigned int /* file_version */) { \
		ar \
		BOOST_PP_SEQ_FOR_EACH(BASE_SERIALIZATION, , bases) \
		BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_SERIALIZATION, , dataMembers); \
	} \
	};


/*************************************************************
 * Helper Macros for boost_pp_for_each macro:
 * elem is a tuple of kind (datatype, name, defaultValue, assignment/return type)
 ************************************************************/

#define DATA_ELEMENT_CONSTR_ARG(r, data, elem) \
	BOOST_PP_TUPLE_ELEM(3,3,elem) BOOST_PP_TUPLE_ELEM(3,0,elem)\

#define DATA_ELEMENT_CONSTR_ARG_COMMA(r, data, elem) \
	, DATA_ELEMENT_CONSTR_ARG(r, data, elem)\

#define DATA_ELEMENT_CONSTR_ASSIGN(r, data, elem) \
	BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem))(BOOST_PP_TUPLE_ELEM(3,0,elem))\

#define DATA_ELEMENT_CONSTR_ASSIGN_COMMA(r, data, elem) \
	, DATA_ELEMENT_CONSTR_ASSIGN(r, data, elem)\

#define DATA_ELEMENT_CONSTR_DEFAULT(r, data, elem) \
	BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem))(BOOST_PP_TUPLE_ELEM(3,2,elem))\

#define DATA_ELEMENT_CONSTR_DEFAULT_COMMA(r, data, elem) \
	, DATA_ELEMENT_CONSTR_DEFAULT(r, data, elem)\

#define DATA_ELEMENT_DECLARATION(r, data, elem) \
	BOOST_PP_TUPLE_ELEM(3,1,elem) BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem));\

#define DATA_ELEMENT_GETTER(r, data, elem) \
	BOOST_PP_TUPLE_ELEM(3,3,elem) BOOST_PP_TUPLE_ELEM(3,0,elem)() {return BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem));}\

#define DATA_ELEMENT_SETTER(r, data, elem) \
	void BOOST_PP_TUPLE_ELEM(3,0,elem)(BOOST_PP_TUPLE_ELEM(3,3,elem) BOOST_PP_TUPLE_ELEM(3,0,elem)) { BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem)) = BOOST_PP_TUPLE_ELEM(3,0,elem);}\

#define DATA_ELEMENT_SERIALIZATION(r, data, elem) \
	& boost::serialization::make_nvp(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(3,0,elem)), BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem)))

#define ADDITIONAL_DECLARATION(r, data, elem) elem

// FOR DERIVED CLASSES

#define BASE_INHERITANCE(r, data, elem) \
	BOOST_PP_TUPLE_ELEM(3,2,elem) BOOST_PP_TUPLE_ELEM(3,1,elem)\

#define BASE_INHERITANCE_COMMA(r, data, elem) \
	, BASE_INHERITANCE(r, data, elem)\

#define BASE_CONSTR_ARG(r, data, elem) \
	BOOST_PP_TUPLE_ELEM(3,1,elem) const& BOOST_PP_TUPLE_ELEM(3,0,elem)\

#define BASE_CONSTR_ARG_COMMA(r, data, elem) \
	, BASE_CONSTR_ARG(r, data, elem)\

#define BASE_CONSTR_ASSIGN(r, data, elem) \
	BOOST_PP_TUPLE_ELEM(3,1,elem)(BOOST_PP_TUPLE_ELEM(3,0,elem))\

#define BASE_CONSTR_ASSIGN_COMMA(r, data, elem) \
	, BASE_CONSTR_ASSIGN(r, data, elem)\

#define BASE_SERIALIZATION(r, data, elem) \
	& boost::serialization::base_object<BOOST_PP_TUPLE_ELEM(3,1,elem)>(*this)\

#endif /* UTILITY_DATACONTAINERMACRO_H_ */
