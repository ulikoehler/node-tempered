{
    "targets": [{
        "target_name": "tempered",
        "sources": ["tempered.cpp"],
	"include_dirs": ["./thirdparty/install/include"],
	"library_dirs": ["./thirdparty/install/lib"],

        "link_settings": {
	    'ldflags': ['-L../thirdparty/install/lib'],
            "libraries": ["-ltempered","-lhidapi-hidraw"],
	    "library_dirs": ["./thirdparty/install/lib"]
        }
    }]
}
