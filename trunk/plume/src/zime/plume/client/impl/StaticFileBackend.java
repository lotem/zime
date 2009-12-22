package zime.plume.client.impl;

import java.util.ArrayList;
import java.util.List;

import com.google.gwt.core.client.GWT;
import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.http.client.URL;
import com.google.gwt.json.client.JSONArray;
import com.google.gwt.json.client.JSONException;
import com.google.gwt.json.client.JSONObject;
import com.google.gwt.json.client.JSONParser;
import com.google.gwt.json.client.JSONString;
import com.google.gwt.json.client.JSONValue;

import zime.plume.client.Backend;
import zime.plume.client.Config;
import zime.plume.client.Query;
import zime.plume.client.QueryResult;
import zime.plume.client.Schema;
import zime.plume.client.util.Logger;

public class StaticFileBackend implements Backend {

    private static final String DATA_DIR = GWT.getHostPageBaseURL() + "data/";
    private static final String SCHEMA_LIST = "SchemaList.json";
    private static final String CONFIG = "Config.json";

    @Override
    public void doQuery(final Query query, final QueryCallback callback) {
    	// TODO
        String url = DATA_DIR;
        String queryString = "schema=" + URL.encodeComponent(query.getSchemaName()) +
                             "&input=" + URL.encodeComponent(query.getInput());
        request(url, queryString, new RequestCallback() {
            @Override
            public void onError(Request request, Throwable exception) {
                callback.onFailure();
            }
            @Override
            public void onResponseReceived(Request request, Response response) {
                // 0 is returned for local static files 
                if (response.getStatusCode() <= Response.SC_OK) {
                    // TODO
                    //response.getText();
                    callback.onReady(new QueryResult(query, null));
                } else {
                    callback.onFailure();
                }
            }
        });
    }

    @Override
    public void loadConfig(final String schema, final LoadConfigCallback callback) {
        String url = DATA_DIR + URL.encodeComponent(schema) + CONFIG; 
        request(url, null, new RequestCallback() {
            @Override
            public void onError(Request request, Throwable exception) {
                callback.onFailure();
            }
            @Override
            public void onResponseReceived(Request request, Response response) {
                // 0 is returned for local static files 
                if (response.getStatusCode() <= Response.SC_OK) {
                    // TODO
                    //response.getText();
                    callback.onReady(new Config("Config/" + schema + "/", null));
                } else {
                    callback.onFailure();
                }
            }
        });
    }

    @Override
    public void loadSchemaList(final LoadSchemaListCallback callback) {
        String url = DATA_DIR + SCHEMA_LIST; 
        request(url, null, new RequestCallback() {
            @Override
            public void onError(Request request, Throwable exception) {
                callback.onFailure();
            }
            @Override
            public void onResponseReceived(Request request, Response response) {
                // 0 is returned for local static files 
                if (response.getStatusCode() <= Response.SC_OK) {
                    List<Schema> list = new ArrayList<Schema>();
                    JSONValue jsonValue = JSONParser.parse(response.getText());
                    JSONArray schemaArray = jsonValue.isArray();
                    if (schemaArray != null) {
                        for (int i = 0; i < schemaArray.size(); ++i) {
                            JSONObject schema = schemaArray.get(i).isObject();
                            if (schema != null) {
                                JSONString schemaName = schema.get("schema").isString();
                                JSONString displayName = schema.get("displayName").isString();
                                if (schemaName != null && displayName != null) {
                                    list.add(new Schema(schemaName.stringValue(), displayName.stringValue()));
                                }
                            }
                        }
                    }
                    callback.onReady(list);
                } else {
                    callback.onFailure();
                }
            }
        });
    }

    private void request(String url, String query, RequestCallback callback) {
        Logger.debug("request: " + url);
        RequestBuilder requestBuilder = new RequestBuilder(RequestBuilder.GET, url);
        try {
            requestBuilder.sendRequest(query, callback);
        } catch (RequestException e) {
            e.printStackTrace();
        }
    }
}
