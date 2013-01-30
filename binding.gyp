{
    "targets": [{
        "target_name": "tempered",
        "sources": ["tempered.cpp"],

        "link_settings": {
            "libraries": ["-ltempered", "-lhidapi-hidraw"]
        }
    }]
}