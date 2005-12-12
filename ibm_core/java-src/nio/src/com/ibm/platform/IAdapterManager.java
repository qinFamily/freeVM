/* Copyright 2004 The Apache Software Foundation or its licensors, as applicable
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.ibm.platform;


/**
 * IAdapterManager
 * 
 */
public interface IAdapterManager {
	public Object getAdapter(IAdaptable adaptable, Class adapterType);

	public boolean hasAdapter(IAdaptable adaptable, Class adapterType);

	public void registerAdapters(IAdapterFactory factory, Class adaptable);

	public void unregisterAdapters(IAdapterFactory factory);

	public void unregisterAdapters(IAdapterFactory factory, Class adaptable);
}
