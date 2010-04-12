package cn.zzsst.zime.client;


import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public abstract class Engine {

    public static final int KEYCODE_APOSTROPHE = 222;
    public static final int KEYCODE_COMMA = 188;
    public static final int KEYCODE_PERIOD = 190;
    public static final int KEYCODE_SPACE = 32;

    protected Module module;
    
    protected Schema schema;

    protected Context context;

    protected CandidateList candidateList;

	public Engine(Module module, Schema schema) {
        this.module = module;
        this.schema = schema;
		this.context = new Context(schema);
		this.candidateList = new CandidateList(null);
	}
	
	public void refreshCandidateList(CandidateList list) {
        if (list == null)
            candidateList.clear();
        else
            candidateList = list;
        module.updateCandidates(candidateList);
	}
	
	public abstract boolean processKeyDownEvent(KeyDownEvent event);

	public abstract boolean processKeyUpEvent(KeyUpEvent event);
	
}
