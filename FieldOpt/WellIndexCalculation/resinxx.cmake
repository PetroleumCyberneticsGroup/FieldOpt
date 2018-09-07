
# RESINXX DIRS =========================================================
set(RESINXX resinxx) 
set(RESINXX_GRID      ${RESINXX}/rixx_grid)
set(RESINXX_APP_FWK   ${RESINXX}/rixx_app_fwk)
set(RESINXX_CORE_GEOM ${RESINXX}/rixx_core_geom)
set(RESINXX_RES_MOD   ${RESINXX}/rixx_res_mod)
set(RESINXX_PRJ_VIZ   ${RESINXX}/rixx_prj_viz)

# MAIN RESINXX FILES ===================================================
set(RIXX_CPP_FILES
${RESINXX}/well_path.cpp
${RESINXX}/geometry_tools.cpp
)

# GRID FILES ===========================================================
set(RIXX_GRID_CPP_FILES
${RESINXX_GRID}/riextractor.cpp
${RESINXX_GRID}/rifaultncc.cpp
${RESINXX_GRID}/ricasedata.cpp
${RESINXX_GRID}/rigrid.cpp
${RESINXX_GRID}/ricell.cpp
)

# APP FWK FILES ========================================================
set(RIXX_APP_FWK_CPP_FILES
${RESINXX_APP_FWK}/cvfStructGrid.cpp
${RESINXX_APP_FWK}/cvfCellRange.cpp
${RESINXX_APP_FWK}/cafHexGridIntersectionTools.cpp
${RESINXX_APP_FWK}/RivSectionFlattner.cpp
)

# CORE GEOM FILES ======================================================
set(RIXX_CORE_GEOM_CPP_FILES
${RESINXX_CORE_GEOM}/cvfAssert.cpp
${RESINXX_CORE_GEOM}/cvfAtomicCounter.cpp
${RESINXX_CORE_GEOM}/cvfBoundingBox.cpp
${RESINXX_CORE_GEOM}/cvfBoundingBoxTree.cpp
${RESINXX_CORE_GEOM}/cvfCharArray.cpp
${RESINXX_CORE_GEOM}/cvfMath.cpp
${RESINXX_CORE_GEOM}/cvfPlane.cpp
${RESINXX_CORE_GEOM}/cvfObject.cpp
${RESINXX_CORE_GEOM}/cvfRay.cpp
${RESINXX_CORE_GEOM}/cvfString.cpp
${RESINXX_CORE_GEOM}/cvfSystem.cpp
${RESINXX_CORE_GEOM}/cvfVector2.cpp
${RESINXX_CORE_GEOM}/cvfVector3.cpp
${RESINXX_CORE_GEOM}/cvfVector4.cpp
)

# RES MOD FILES ========================================================
set(RIXX_RES_MOD_CPP_FILES
${RESINXX_RES_MOD}/cvfGeometryTools.cpp
${RESINXX_RES_MOD}/RigCellGeometryTools.cpp
)

# PRJ MOD FILES ========================================================
set(RIXX_PRJ_VIZ_CPP_FILES
#${RESINXX_PRJ_VIZ}/cvfDrawable.cpp
#${RESINXX_PRJ_VIZ}/cvfDrawableGeo.cpp
#${RESINXX_PRJ_VIZ}/cvfPrimitiveSet.cpp
#${RESINXX_PRJ_VIZ}/cvfPrimitiveSetIndexedUInt.cpp
#${RESINXX_PRJ_VIZ}/cvfPrimitiveSetIndexedUIntScoped.cpp
#${RESINXX_PRJ_VIZ}/cvfPrimitiveSetIndexedUShort.cpp
#${RESINXX_PRJ_VIZ}/cvfPrimitiveSetIndexedUShortScoped.cpp
#${RESINXX_PRJ_VIZ}/cvfVertexAttribute.cpp
#${RESINXX_PRJ_VIZ}/cvfVertexBundle.cpp
#${RESINXX_PRJ_VIZ}/cvfVertexWelder.cpp
${RESINXX_PRJ_VIZ}/RigFemPart.cpp
${RESINXX_PRJ_VIZ}/RigFemPartGrid.cpp
${RESINXX_PRJ_VIZ}/RigFemTypes.cpp
${RESINXX_PRJ_VIZ}/RimIntersection.cpp
${RESINXX_PRJ_VIZ}/RivHexGridIntersectionTools.cpp
${RESINXX_PRJ_VIZ}/RivIntersectionGeometryGenerator.cpp
${RESINXX_PRJ_VIZ}/RivIntersectionPartMgr.cpp
${RESINXX_PRJ_VIZ}/RivIntersectionSourceInfo.cpp
)

message(".............................................................")
message("RIXX_CPP_FILES: ${RIXX_CPP_FILES}")
message(".............................................................")
message("RIXX_GRID_CPP_FILES: ${RIXX_GRID_CPP_FILES}")
message(".............................................................")
message("RIXX_APP_FWK_CPP_FILES: ${RIXX_APP_FWK_CPP_FILES}")
message(".............................................................")
message("RIXX_CORE_GEOM_CPP_FILES: ${RIXX_CORE_GEOM_CPP_FILES}")
message(".............................................................")
message("RIXX_RES_MOD_CPP_FILES: ${RIXX_RES_MOD_CPP_FILES}")
message(".............................................................")
message("RIXX_PRJ_VIZ_CPP_FILES: ${RIXX_PRJ_VIZ_CPP_FILES}")

# ALL ==================================================================
set(RIXX_ALL_CPP_FILES
${RIXX_CPP_FILES}
${RIXX_GRID_CPP_FILES}
${RIXX_APP_FWK_CPP_FILES}
${RIXX_CORE_GEOM_CPP_FILES}
${RIXX_RES_MOD_CPP_FILES}
${RIXX_PRJ_VIZ_CPP_FILES}
)