#pragma once
#include "box_view.hpp"

namespace mbmff {

//------------------------------------------------------------------------------------------------------------
// Pure container boxes — no payload fields, just child boxes.
// All share the same pattern: container property, always-passes validate, empty value().
//------------------------------------------------------------------------------------------------------------

#define MBMFF_DEFINE_PURE_CONTAINER(box)                                                      \
    struct box##_data {};                                                                     \
                                                                                              \
    template <>                                                                               \
    struct basic_box_view<mbmff::box_type::box> : public mbmff::box_view_base {               \
        constexpr static mbmff::box_properties properties = mbmff::box_properties::container; \
    }

MBMFF_DEFINE_PURE_CONTAINER(moov);
MBMFF_DEFINE_PURE_CONTAINER(trak);
MBMFF_DEFINE_PURE_CONTAINER(mdia);
MBMFF_DEFINE_PURE_CONTAINER(minf);
MBMFF_DEFINE_PURE_CONTAINER(stbl);
MBMFF_DEFINE_PURE_CONTAINER(dinf);
MBMFF_DEFINE_PURE_CONTAINER(edts);
MBMFF_DEFINE_PURE_CONTAINER(gmhd);
MBMFF_DEFINE_PURE_CONTAINER(tref);
MBMFF_DEFINE_PURE_CONTAINER(mvex);
MBMFF_DEFINE_PURE_CONTAINER(udta);
MBMFF_DEFINE_PURE_CONTAINER(moof);
MBMFF_DEFINE_PURE_CONTAINER(traf);
MBMFF_DEFINE_PURE_CONTAINER(mfra);
MBMFF_DEFINE_PURE_CONTAINER(iprp);
MBMFF_DEFINE_PURE_CONTAINER(ipco);
MBMFF_DEFINE_PURE_CONTAINER(ilst);
MBMFF_DEFINE_PURE_CONTAINER(meco);
MBMFF_DEFINE_PURE_CONTAINER(mere);
MBMFF_DEFINE_PURE_CONTAINER(ipro);
MBMFF_DEFINE_PURE_CONTAINER(sinf);
MBMFF_DEFINE_PURE_CONTAINER(fiin);
MBMFF_DEFINE_PURE_CONTAINER(paen);
MBMFF_DEFINE_PURE_CONTAINER(strk);
MBMFF_DEFINE_PURE_CONTAINER(tapt);
MBMFF_DEFINE_PURE_CONTAINER(schi);

#undef MBMFF_DEFINE_PURE_CONTAINER

#ifdef MBMFF_ENABLE_CONSTEXPR_TEST
static_assert(
    mbmff::basic_box_view<mbmff::box_type::moov>::validate({mbmff::box_header{}, std::span<const std::byte>{}})
);
#endif

} // namespace mbmff
