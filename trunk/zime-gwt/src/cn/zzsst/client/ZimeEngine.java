package cn.zzsst.client;


import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public abstract class ZimeEngine {

    public static final int KEYCODE_APOSTROPHE = 222;
    public static final int KEYCODE_COMMA = 188;
    public static final int KEYCODE_PERIOD = 190;
    public static final int KEYCODE_SPACE = 32;

	protected ZimeModule module;
	
	protected Schema schema;

    protected Context context;

    protected CandidateList candidateList;

	public ZimeEngine(ZimeModule module, Schema schema) {
		this.module = module;
		this.schema = schema;
		this.context = new Context(schema);
		this.candidateList = new CandidateList();
	}
	
	public abstract boolean processKeyDownEvent(KeyDownEvent event);

	public abstract boolean processKeyUpEvent(KeyUpEvent event);
	
}
