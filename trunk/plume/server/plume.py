from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

import simplejson as json
import plumedb


class SchemaListHandler(webapp.RequestHandler):
    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.out.write(json.dumps([u'TODO']))

class SchemaHandler(webapp.RequestHandler):
    def get(self, schema):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.out.write(json.dumps([u'TODO']))

class QueryHandler(webapp.RequestHandler):
    def get(self, schema):
        self.post(schema)
    def post(self, schema):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.out.write(json.dumps([u'TODO']))

class CommitHandler(webapp.RequestHandler):
    def get(self, schema):
        self.post(schema)
    def post(self, schema):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.out.write(json.dumps([u'TODO']))


application = webapp.WSGIApplication([(r'/schema_list', SchemaListHandler),
                                      (r'/schema/(.*)', SchemaHandler),
                                      (r'/query/(.*)', QueryHandler),
                                      (r'/commit/(.*)', CommitHandler)
                                     ],
                                     debug=True)

def main():
    run_wsgi_app(application)

if __name__ == '__main__':
    main()
