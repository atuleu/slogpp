set(HDR_FILES_LOCAL #
	Logger.hpp Logger.imp.hpp #
	Sink.hpp Sink.imp.hpp #
)

set(HDR_FILES "")
foreach(f ${HDR_FILES_LOCAL})
	list(APPEND HDR_FILES include/slog++/${f})
endforeach(f ${HDR_FILES_LOCAL})

set(HDR_FILES
	${HDR_FILES}
	PARENT_SCOPE
)
