package cn.zzsst.zime.client;

import java.util.ArrayList;
import java.util.HashMap;

public class StaticFileDict implements Dict {

    private static final String DIRECTIVE_KEY = "%key ";
    
    private String dictPrefix;
    private HashMap<String, LookupRequest> pending;
    private HashMap<String, ArrayList<String>> index;

    public StaticFileDict(String codeName) {
        dictPrefix = StaticFileCallback.PREFIX + codeName + "/dict/";
        pending = new HashMap<String, LookupRequest>();
        index = new HashMap<String, ArrayList<String>>(); 
        new StaticFileCallback() {
            @Override
            public void onSuccess(String text) {
                try {
                    String[] lines = text.split("\\n+");
                    for (String x : lines) {
                        index.put(x, null);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            @Override
            public void onFailure(Throwable e) {
                Logger.log("missing index file.");
            }
        }.fetch(StaticFileCallback.PREFIX + codeName + "/index.txt");
    }
    
    private class LookupRequest extends StaticFileCallback {
        private String id;
        private HashMap<String, LookupCallback> callbacks;

        public LookupRequest(String id) {
            this.id = id;
            callbacks = new HashMap<String, LookupCallback>();
        }
        
        public void add(String key, LookupCallback callback) {
            callbacks.put(key, callback);
        }

        @Override
        public void onSuccess(String text) {
            populate(text);
            release(id);
            for (String k : callbacks.keySet()) {
                //Logger.log("LookupRequest.onSuccess: " + k);
                callbacks.get(k).onSuccess(new CandidateList(index.get(k)));
            }
        }

        @Override
        public void onFailure(Throwable e) {
            release(id);
            for (LookupCallback callback : callbacks.values())
                callback.onFailure(e);
        }

        private void populate(String text) {
            String key = null;
            ArrayList<String> list = new ArrayList<String>();
            try {
                String[] lines = text.split("\\n+");
                for (String x : lines) {
                    if (x.startsWith(DIRECTIVE_KEY)) {
                        if (key != null) {
                            index.put(key, list);
                            list = new ArrayList<String>();
                        }
                        key = x.substring(DIRECTIVE_KEY.length());
                        continue;
                    }
                    list.add(x);
                }
                if (key != null) {
                    index.put(key, list);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void lookup(String key, LookupCallback callback) {
        //Logger.log("StaticFileDict.lookup(): " + key);
        // check phrase index
        if (!index.containsKey(key)) {
            int pos = key.indexOf('/');
            if (pos == -1) {
                // not even a word
                callback.onFailure(null);
            }
            else {
                // lookup phrases if head word has not been looked up
                String head = key.substring(0, pos);
                if (index.get(head) == null) {
                    request(head + "-phrase").add(key, callback);
                }
            }
            return;
        }
        // check cache
        ArrayList<String> list = index.get(key);
        if (list != null) {
            callback.onSuccess(new CandidateList(list));
            return;
        }
        // lookup single words and prefetch phrases 
        if (key.indexOf('/') == -1) {
            request(key).add(key, callback);
            request(key + "-phrase");
        }
	}

    private LookupRequest request(String requestId) {
        //Logger.log("StaticFileDict.request: " + requestId);
        LookupRequest result;
        if (pending.containsKey(requestId))
            result = pending.get(requestId);
        else {
            result = new LookupRequest(requestId);
            pending.put(requestId, result);
            String filePath = dictPrefix + requestId + ".txt";
            //Logger.log("    fetchStaticFile: " + filePath);
            result.fetch(filePath);
        }
        return result;
    }
    
    private void release(String requestId) {
        pending.remove(requestId);
    }

    @Override
    public boolean exist(String key) {
        return index.containsKey(key);
    }

}
