package com.mattcorallo.relaynode;

import javax.annotation.Nonnull;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.net.InetAddress;

/**
 * Java's getHostName() does a hostname lookup to "verify" RDNS records before returning them,
 * but we're dealing with lots of servers that dont have forward records and want to see the
 * results anyway, so we reach into the JDK and force it to give us what we want....
 */
public class RDNS {
	public static String getRDNS(@Nonnull InetAddress addr) {
		if (!addr.toString().equals("/" + addr.getHostAddress()) && !addr.toString().equals(addr.getHostAddress() + "/" + addr.getHostAddress()))
			return addr.getHostName(); // Use hostname originally used to resolve the address
		try {
			Field impl = InetAddress.class.getDeclaredField("impl");
			impl.setAccessible(true);
			Method getHostByAddr = impl.getType().getDeclaredMethod("getHostByAddr", byte[].class);
			getHostByAddr.setAccessible(true);
			Object res = getHostByAddr.invoke(impl.get(null), addr.getAddress());
			return (String)res;
		} catch (Exception e) {
			return addr.getCanonicalHostName();
		}
	}

}
