package cn.zzsst.client;

import java.util.ArrayList;
import java.util.HashMap;

import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.user.client.Timer;


public class DummyDict implements Dict {

    private HashMap<String, ArrayList<String>> index;
    private String dictPath;

    public DummyDict(String codeName) {
        
        index = new HashMap<String, ArrayList<String>>(); 
        dictPath = "../schema/" + codeName + "/dummy-dict.txt";
        
        RequestCallback callback = new RequestCallback() {

            @Override
            public void onError(Request request, Throwable e) {
                e.printStackTrace();
            }

            @Override
            public void onResponseReceived(Request request, Response response) {
                loadIndex(response.getText());
            }
            
        };
        fetchStaticFile(dictPath, callback);
    }

    private void fetchStaticFile(String filePath, RequestCallback callback) {
        RequestBuilder requestBuilder = new RequestBuilder(RequestBuilder.GET, filePath);
        try {
            requestBuilder.sendRequest(null, callback);
        } catch (RequestException e) {
            e.printStackTrace();
        }
    }

    protected void loadIndex(String text) {
        try {
            String[] lines = text.split("\n");
            for (String line : lines) {
                String[] r = line.split("[ \\t]+", 2);
                final String key = r[0]; 
                ArrayList<String> list;
                if (index.containsKey(key)) {
                    list = index.get(key);
                } else {
                    list = new ArrayList<String>();
                    index.put(key, list);
                }
                list.add(r[1]);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private class DummyDictTimer extends Timer {

        protected String key;
        protected Callback callback;

        public DummyDictTimer(String key, Callback callback) {
            this.key = key;
            this.callback = callback;
        }

        @Override
        public void run() {
            callback.onReady(index.get(key));
        }
    }
    
    @Override
    public void lookup(String key, Callback callback) {
        System.err.println("DummyDict.lookup(): " + key);
        // TODO
        Timer t = new DummyDictTimer(key, callback);
        t.schedule(2000);
	}

    @Override
    public boolean exist(String key) {
        return index.containsKey(key);
    }

}
