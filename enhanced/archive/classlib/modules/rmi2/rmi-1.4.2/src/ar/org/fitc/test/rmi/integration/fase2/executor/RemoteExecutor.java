/*
 *  Copyright 2005 The Apache Software Foundation or its licensors, as applicable.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
package ar.org.fitc.test.rmi.integration.fase2.executor;

import java.rmi.RemoteException;

import ar.org.fitc.test.rmi.integration.fase2.interfaces.ITCRemote;

/**
 * Provides the methods for implements remote execution. 
 * 
 * @author Jorge Rafael
 * @author Marcelo Arcidiacono
 * 
 * @version 1.0
 */
public interface RemoteExecutor extends ITCRemote {

	/**
	 * A generic method that receives a <code>ServerExecutor</code> 
	 * a number of executions and an arbitrary number of arguments 
	 * and return a generic object.
	 *  
	 * @param obj a <code>ServerExecutor</code>
	 * @param times of the executions
	 * @param arguments an arbitrary number of arguments
	 * @return a generic object
	 * @throws RemoteException if the remote operation fails
	 */
	public Object execute(ServerExecutor obj, int times, Object... arguments)
			throws RemoteException;

	/**
	 * A generic method that receives a <code>ServerExecutor</code> 
	 * a number of executions and an arbitrary number of arguments 
	 * and return a thread.
	 *  
	 * @param ser a <code>ServerExecutor</code>
	 * @param times of the executions 
	 * @param arguments an arbitrary number of arguments
	 * @return a thread
	 * @throws RemoteException if the remote operation fails
	 */
	public Thread threadedExecute(final ServerExecutor ser, final int times,
			final Object... arguments) throws RemoteException;
}