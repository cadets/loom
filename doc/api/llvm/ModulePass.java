package llvm;

/**
 * @opt nodefillcolor #cccccc
 */
public class ModulePass
{
	public abstract boolean runOnModule(llvm.Module Mod);
}
