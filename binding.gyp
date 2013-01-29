{
    "targets": [{
        "target_name": "tempered",
        "sources": ["tempered.cpp"],

        "link_settings": {
            "libraries": ["-ltempered", "-ltempered-util", "-lhidapi-hidraw"]
        }
    }]
}