#ifndef UTILITY_DATACONTAINERMACRO_H_
#define UTILITY_DATACONTAINERMACRO_H_

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/first_n.hpp>
#include <boost/preprocessor/seq/rest_n.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#include <boost/serialization/nvp.hpp>

#define DATA_CONTAINER(classname,variables,additionalDecl) \
	class classname { \
	public: \
	\
	classname(\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ARG, , BOOST_PP_SEQ_FIRST_N(1,variables))\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ARG_COMMA, , BOOST_PP_SEQ_REST_N(1,variables))\
	) :\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ASSIGN, , BOOST_PP_SEQ_FIRST_N(1,variables))\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_ASSIGN_COMMA, , BOOST_PP_SEQ_REST_N(1,variables))\
	{}\
	\
	classname() :\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_DEFAULT, , BOOST_PP_SEQ_FIRST_N(1,variables))\
			BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_CONSTR_DEFAULT_COMMA, , BOOST_PP_SEQ_REST_N(1,variables))\
	{}\
	\
	BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_GETTER, , variables) \
	\
	BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_SETTER, , variables) \
	\
	BOOST_PP_SEQ_FOR_EACH(ADDITIONAL_DECLARATION, , additionalDecl) \
	\
	private: \
	\
	BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_DECLARATION, , variables) \
	\
	friend class boost::serialization::access; \
	template<class Archive> \
	void serialize(Archive & ar, const unsigned int /* file_version */) { \
		ar BOOST_PP_SEQ_FOR_EACH(DATA_ELEMENT_SERIALIZATION, , variables); \
	} \
	}; \
\

/*************************************************************
 * Helper Macros for boost_pp_for_each macro:
 * elem is a tuple of kind (datatype, name, defaultValue, assignment/return type)
 ************************************************************/

#define DATA_ELEMENT_CONSTR_ARG(r, data, elem) \
	BOOST_PP_TUPLE_ELEM(3,3,elem) BOOST_PP_TUPLE_ELEM(3,0,elem)\

#define DATA_ELEMENT_CONSTR_ARG_COMMA(r, data, elem) \
	, BOOST_PP_TUPLE_ELEM(3,3,elem) BOOST_PP_TUPLE_ELEM(3,0,elem)\

#define DATA_ELEMENT_CONSTR_ASSIGN(r, data, elem) \
	BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem))(BOOST_PP_TUPLE_ELEM(3,0,elem))\

#define DATA_ELEMENT_CONSTR_ASSIGN_COMMA(r, data, elem) \
	, BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem))(BOOST_PP_TUPLE_ELEM(3,0,elem))\

#define DATA_ELEMENT_CONSTR_DEFAULT(r, data, elem) \
	BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem))(BOOST_PP_TUPLE_ELEM(3,2,elem))\

#define DATA_ELEMENT_CONSTR_DEFAULT_COMMA(r, data, elem) \
	, BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem))(BOOST_PP_TUPLE_ELEM(3,2,elem))\

#define DATA_ELEMENT_DECLARATION(r, data, elem) \
	BOOST_PP_TUPLE_ELEM(3,1,elem) BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem));\

#define DATA_ELEMENT_GETTER(r, data, elem) \
	BOOST_PP_TUPLE_ELEM(3,3,elem) BOOST_PP_TUPLE_ELEM(3,0,elem)() {return BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem));}\

#define DATA_ELEMENT_SETTER(r, data, elem) \
	void BOOST_PP_TUPLE_ELEM(3,0,elem)(BOOST_PP_TUPLE_ELEM(3,3,elem) BOOST_PP_TUPLE_ELEM(3,0,elem)) { BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem)) = BOOST_PP_TUPLE_ELEM(3,0,elem);}\

#define DATA_ELEMENT_SERIALIZATION(r, data, elem) \
	& boost::serialization::make_nvp(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(3,0,elem)), BOOST_PP_CAT(m_, BOOST_PP_TUPLE_ELEM(3,0,elem)))

#define ADDITIONAL_DECLARATION(r, data, elem) elem

#endif /* UTILITY_DATACONTAINERMACRO_H_ */
