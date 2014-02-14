#ifndef DENALI_CONCEPTS_COMMON_H
#define DENALI_CONCEPTS_COMMON_H

namespace denali {
    namespace concepts {

        /// \brief A concept modeling no functionality or constraints.
        /// \ingroup concepts_common
        /*!
         *  Concept classes are implemented as mixins: they extend the archetypes
         *  of their base concepts. As such, an empty concept class is necessary
         *  for composing layers of concepts.
         */
        class Null
        {
            template <typename _Null>
            struct Constraints
            {
                void constraints() {}
            };
        };

    }
}

#endif
