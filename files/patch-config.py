--- config.py.orig	2014-09-15 12:48:22.566709487 -0400
+++ config.py	2014-09-15 13:11:44.042621405 -0400
@@ -73,13 +73,6 @@ logging = {
             'backupCount': 2,
             'formatter': 'standard'
         },
-        'logstash': {
-            'class': 'logstash.LogstashHandler',
-            'level': 'INFO',
-            'host': 'localhost',
-            'port': 5000,
-            'version': 1
-        }
     },
     'formatters': {
         'standard': {
