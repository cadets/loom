/**
 * @opt edgefontname "Avenir"
 * @opt edgefontsize 10
 *
 * @opt nodefontname "Avenir"
 * @opt nodefontsize 12
 *
 * @opt nodefillcolor "#ddddff"
 * @hidden
 */
class UMLOptions{}


/** @navassoc "" - "" llvm.Module */
class DebugInfo
{
}

/**
 * @navassoc "" - "" InstrStrategy
 * @navassoc "" - "*" Instrumentation
 * @navassoc "" - "" llvm.Module
 */
class Instrumenter
{
	public boolean Instrument(llvm.Instruction I) {}
	public boolean Instrument(llvm.CallInst Call) {}
	public boolean Instrument(llvm.Function F) {}
	public boolean Instrument(llvm.GetElementPtrInst GEP, llvm.LoadInst L) {}
	public boolean Instrument(llvm.GetElementPtrInst GEP, llvm.StoreInst S) {}
}

/**
 * @depend - <creates> - Instrumentation
 * @depend - <uses> - Logger
 */
class InstrStrategy
{
	public enum Kind {}
	public void AddLogger(Logger Log) {}

	public Instrumentation Instrument(llvm.Instruction I, llvm.Value[] Values) {}

	private boolean UseBlockStructure;
}

/**
 * @navassoc "" - "Preamble,EndBlock" llvm.BasicBlock
 * @navassoc "" - "PreambleEnd,End" llvm.Instruction
 */
class Instrumentation
{
	private llvm.Value[] InstrValues;
}

/**
 * @navassoc "" - "" Serializer
 */
class KTraceLogger extends Logger
{
	public llvm.Value Log(llvm.Instruction I, llvm.Value[] Values,
	                      String Name, String Description,
	                      boolean SuppressUniqueness);

}

class LibxoLogger extends SimpleLogger
{
	public llvm.Value CreateFormatString(Parameter[] Params) {}
}

/**
 * @depend - <uses> - llvm.Function.ArgumentListType
 * @navassoc "" - "" llvm.Module
 */
class Logger
{
	public abstract llvm.Value Log(llvm.Instruction I, llvm.Value[] Values,
	                               String Name, String Description,
	                               boolean SuppressUniqueness);

	public llvm.Value Log(llvm.Instruction I,
	                      llvm.Function.ArgumentListType Args,
	                      String Name, String Description,
	                      boolean SuppressUniqueness)
	{
	}
}

/**
 * @depend - <uses> - Serializer.BufferInfo
 */
class NVSerializer extends Serializer
{
	public BufferInfo Serialize(llvm.Value[] Values) {}
	public llvm.Value Cleanup(BufferInfo Buffer) {}
}

class NullSerializer extends Serializer
{
	public BufferInfo Serialize(llvm.Value[] Values) {}
	public llvm.Value Cleanup(BufferInfo Buffer) {}
}

/**
 * @navassoc "" - "" PolicyFile
 * @depend - <creates> - InstrStrategy
 * @depend - <uses> - Instrumenter
 */
class OptPass extends llvm.ModulePass
{
	public boolean runOnModule(llvm.Module Mod) {}
}

/**
 * @navassoc "" - "" llvm.Type
 */
class Parameter
{
	String Name;
}

/**
 * @depend - <creates> - Logger
 */
interface Policy
{
  public InstrStrategy.Kind Strategy();
  public Logger[] Loggers(llvm.Module Mod);
  public SimpleLogger.LogType Logging();

  public enum KTraceTarget { Kernel, Userspace, None }

  public KTraceTarget KTrace();

  public Serializer Serialization(llvm.Module Mod);

  public boolean UseBlockStructure();

  public boolean InstrumentAll();

  public enum Direction { In, Out };

  public Direction[] CallHooks(llvm.Function F);
  public Direction[] FnHooks(llvm.Function F);

  public boolean StructTypeMatters(llvm.StructType S);

  public boolean
    FieldReadHook(llvm.StructType T, String Field);

  public boolean
    FieldWriteHook(llvm.StructType T, String Field);

  public String InstrName(String[] Components);
}

class PolicyFile implements Policy
{
  public InstrStrategy.Kind Strategy() {}
  public Logger[] Loggers(llvm.Module Mod) {}
  public SimpleLogger.LogType Logging() {}
  public KTraceTarget KTrace() {}
  public Serializer Serialization(llvm.Module Mod) {}
  public boolean UseBlockStructure() {}
  public boolean InstrumentAll() {}
  public Direction[] CallHooks(llvm.Function F) {}
  public Direction[] FnHooks(llvm.Function F) {}
  public boolean StructTypeMatters(llvm.StructType S) {}
  public boolean
    FieldReadHook(llvm.StructType T, String Field) {}
  public boolean
    FieldWriteHook(llvm.StructType T, String Field) {}
  public String InstrName(String[] Components) {}
}

class PrintfLogger extends SimpleLogger
{
	public llvm.Value CreateFormatString(Parameter[] Params) {}
}

/**
 * @depend - <uses> - Parameter
 */
class SimpleLogger extends Logger
{
	public enum LogType {}

	protected abstract llvm.Value CreateFormatString(Parameter[] Params);
	protected llvm.Value CreateFormatString(llvm.Value[] Values) {}

	protected llvm.Value[] Adapt(llvm.Value[] Values) {}
}

class Serializer
{
	public class BufferInfo
	{
		llvm.Value Pointer;
		llvm.Value Length;
	}

	public abstract BufferInfo Serialize(llvm.Value[] Values);
	public abstract llvm.Value Cleanup(BufferInfo Buffer);
}
