from google.appengine.ext import webapp
from google.appengine.ext.webapp.util import run_wsgi_app

import simplejson as json
import plumedb
import plumemodel

db = plumedb.DB()
model = plumemodel.Model(db)

class SchemaListHandler(webapp.RequestHandler):
    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.out.write(json.dumps(db.get_schema_list(), indent=2))

class SchemaHandler(webapp.RequestHandler):
    def get(self, schema):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.out.write(json.dumps(db.get_schema(schema), indent=2))

class QueryHandler(webapp.RequestHandler):
    def post(self, schema):
        m = json.loads(self.request.get('m'))
        b = json.loads(self.request.get('b'))
        e = json.loads(self.request.get('e'))
        result = model.query(m, b, e)
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.out.write(json.dumps(result))

class CommitHandler(webapp.RequestHandler):
    def post(self, schema):
        # TODO:
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.out.write('OK')


application = webapp.WSGIApplication([(r'/plume/schema_list', SchemaListHandler),
                                      (r'/plume/schema/(.*)', SchemaHandler),
                                      (r'/plume/query/(.*)', QueryHandler),
                                      (r'/plume/commit/(.*)', CommitHandler)
                                     ],
                                     debug=True)

def main():
    run_wsgi_app(application)

if __name__ == '__main__':
    main()
