package cn.zzsst.client;

import java.util.ArrayList;
import java.util.HashMap;

import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;


public class DummyDict implements Dict {

    private HashMap<String, ArrayList<String>> index;
    private String dictPath;

    public DummyDict(String codeName) {
        
        index = new HashMap<String, ArrayList<String>>(); 
        dictPath = "../schema/" + codeName + "/dummy-dict.txt";
        
        RequestBuilder requestBuilder = new RequestBuilder(RequestBuilder.GET, dictPath);
        try {
            requestBuilder.sendRequest(null, new RequestCallback() {

                @Override
                public void onError(Request request, Throwable exception) {
                    // TODO Auto-generated method stub
                    
                }

                @Override
                public void onResponseReceived(Request request, Response response) {
                    loadIndex(response.getText());
                }
                
            });
        } catch (RequestException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    protected void loadIndex(String text) {
        try {
            String[] lines = text.trim().split("\n");
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
            // TODO: handle exception
        }
    }

    @Override
    public void lookup(String key, Callback callback) {
        System.err.println("DummyDict.lookup(): " + key);
		callback.onReady(index.get(key));
	}

    @Override
    public boolean exist(String key) {
        return index.containsKey(key);
    }

}
