include ../Makefile.in.$(shell uname)

CXXFLAGS= $(AGGCXXFLAGS) -I../include -L./

SRC_CXX=\
agg_affine_matrix.cpp \
agg_arc.cpp \
agg_arrowhead.cpp \
agg_bezier_arc.cpp \
agg_bspline.cpp \
agg_curves.cpp \
agg_gen_contour.cpp \
agg_gen_dash.cpp \
agg_gen_markers_term.cpp \
agg_gen_smooth_poly1.cpp \
agg_gen_stroke.cpp \
agg_gsv_text.cpp \
agg_image_filters.cpp \
agg_path_storage.cpp \
agg_rasterizer_scanline_aa.cpp \
agg_line_aa_basics.cpp \
agg_line_profile_aa.cpp \
agg_rounded_rect.cpp \
agg_scanline_u8.cpp \
agg_sqrt_tables.cpp \
agg_embedded_raster_fonts.cpp \
agg_trans_warp_magnifier.cpp \
agg_vpgen_clip_polygon.cpp \
agg_vpgen_segmentator.cpp \
ctrl/agg_cbox_ctrl.cpp \
ctrl/agg_gamma_ctrl.cpp \
ctrl/agg_gamma_spline.cpp \
ctrl/agg_rbox_ctrl.cpp \
ctrl/agg_slider_ctrl.cpp \
ctrl/agg_spline_ctrl.cpp \
ctrl/agg_scale_ctrl.cpp

SRC_C=\
../gpc/gpc.c


OBJ=$(SRC_CXX:.cpp=.o) $(SRC_C:.c=.o)

all:	$(OBJ)
	$(LIB) libagg.a $(OBJ) 

clean:
	rm -f *.o *.a ctrl/*.o ../gpc/*.o
	rm -rf SunWS_cache
	rm -rf ctrl/SunWS_cache

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $*.cpp -o $@

%.o: %.c
	$(C) -c $(CXXFLAGS) $*.c -o $@
