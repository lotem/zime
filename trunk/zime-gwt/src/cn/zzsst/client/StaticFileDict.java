package cn.zzsst.client;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;

import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;


public class StaticFileDict implements Dict {

    private String prefix;
    private HashSet<String> index;

    public StaticFileDict(String codeName) {
        prefix = "../schema/" + codeName;
        index = new HashSet<String>(); 
        fetchStaticFile(prefix + "/index.txt", new StaticFileCallback() {
            @Override
            public void onReady(String text) {
                populate(index, text);
            }
            @Override
            public void onFailure() {
            }
        });
    }
    
    private class StaticFileDictLookupCallback extends StaticFileCallback {
        protected String key;
        protected Callback callback;

        public StaticFileDictLookupCallback(String key, Callback callback) {
            this.key = key;
            this.callback = callback;
        }

        @Override
        public void onReady(String text) {
            ArrayList<String> result = new ArrayList<String>();
            populate(result, text);
            callback.onReady(result);                
        }
        @Override
        public void onFailure() {
            callback.onReady(null);
        }
    }

    @Override
    public void lookup(String key, Callback callback) {
        System.err.println("StaticFile.lookup(): " + key);
        // check phrase index
        if (!index.contains(key)) {
            callback.onReady(null);
            return;
        }
        fetchStaticFile(getDictFileName(key), 
                new StaticFileDictLookupCallback(key, callback));
	}

    private String getDictFileName(String key) {
        return prefix + "/dict/" + key + ".txt";
    }

    @Override
    public boolean exist(String key) {
        return index.contains(key);
    }

    private void fetchStaticFile(String filePath, RequestCallback callback) {
        RequestBuilder requestBuilder = new RequestBuilder(RequestBuilder.GET, filePath);
        try {
            requestBuilder.sendRequest(null, callback);
        } catch (RequestException e) {
            e.printStackTrace();
        }
    }

    private void populate(Collection<String> cont, String text) {
        try {
            String[] lines = text.split("\n");
            for (String x : lines) {
                cont.add(x);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
}
