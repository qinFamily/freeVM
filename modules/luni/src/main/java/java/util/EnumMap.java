/* Copyright 2006 The Apache Software Foundation or its licensors, as applicable
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

package java.util;

import java.io.Serializable;

import org.apache.harmony.luni.util.NotYetImplementedException;

public class EnumMap<K, V> extends AbstractMap<K, V> implements Map<K, V>,
        Serializable, Cloneable {
    private static final long serialVersionUID = 458661240069192865L;

    /**
     * Constructs an empty enum map using the given key type.
     * 
     * @param keyType
     *            the class object of the key type used by this enum map
     * @throws NullPointerException
     *             if the keyType is null
     */
    public EnumMap(Class<K> keyType) {
        throw new NotYetImplementedException();
    }

    /**
     * Constructs an enum map using the same key type as the given enum map and
     * initially containing the same mappings.
     * 
     * @param map
     *            the enum map from which this enum map is initialized
     * @throws NullPointerException
     *             if the map is null
     */
    public EnumMap(EnumMap<K, ? extends V> map) {
        throw new NotYetImplementedException();
    }

    /**
     * Constructs an enum map initialized from the given map. If the given map
     * is an EnumMap instance, this constructor behaves in the exactly the same
     * way as {@link EnumMap#EnumMap(EnumMap)}}. Otherwise, the given map at
     * least should contain one mapping.
     * 
     * @param map
     *            the map from which this enum map is initialized
     * @throws IllegalArgumentException
     *             if the map is not an enum map instance and does not contain
     *             any mappings
     * @throws NullPointerException
     *             if the map is null
     */
    public EnumMap(Map<K, ? extends V> map) {
        throw new NotYetImplementedException();
    }

    /**
     * Removes all mappings in this map.
     */
    public void clear() {
        throw new NotYetImplementedException();
    }

    /**
     * Answers a shallow copy of this map.
     * 
     * @return a shallow copy of this map
     */
    public EnumMap<K,V> clone() {
        throw new NotYetImplementedException();
    }

    /**
     * Answers true if this map has a mapping for the given key.
     * 
     * @param key
     *            the key whose presence in this map is to be tested
     * @return true if this map has a mapping for the given key.
     */
    public boolean containsKey(Object key) {
        throw new NotYetImplementedException();
    }

    /**
     * Answers true if this map has one or more keys mapped to the given value.
     * 
     * @param value
     *            the value whose presence in this map is to be tested
     * @return true if this map has one or more keys mapped to the given value.
     */
    public boolean containsValue(Object value) {
        throw new NotYetImplementedException();
    }

    /**
     * Answers a {@link Set}} view of the mappings contained in this map. The
     * returned set complies with the general rule specified in
     * {@link Map#entrySet()}}. The set's iterator will return the mappings in
     * the their keys' natural order(the enum constants are declared in this
     * order)
     * 
     * @return a set view of the mappings contained in this map.
     */
    public Set<Map.Entry<K, V>> entrySet() {
        throw new NotYetImplementedException();
    }

    /**
     * Compares the given object with this map. Answers true if the given object
     * is equal to this map.
     * 
     * @param object
     *            the object to be compared with this map
     * @return true if the given object is equal to this map.
     */
    public boolean equals(Object object) {
        throw new NotYetImplementedException();
    }

    /**
     * Answers the value which is mapped to the given key in this map, or null
     * if this map has no mapping for the given key.
     * 
     * @param key
     *            the key whose associated value is to be returned
     * @return the value to which this map maps the given key, or null if this
     *         map has no mapping for the given key.
     */
    public V get(Object key) {
        throw new NotYetImplementedException();
    }

    /**
     * Answers a {@link Set}} view of the keys contained in this map. The
     * returned set complies with the general rule specified in
     * {@link Map#keySet()}}. The set's iterator will return the keys in the
     * their natural order(the enum constants are declared in this order)
     * 
     * @return a set view of the keys contained in this map.
     */
    public Set<K> keySet() {
        throw new NotYetImplementedException();
    }

    /**
     * Associates the given value with the given key in this map. If the map
     * previously had a mapping for this key, the old value is replaced.
     * 
     * @param key
     *            the key with which the given value is to be associated value
     * @param value
     *            the value to be associated with the given key
     * @return the value to which this map maps the given key, or null if this
     *         map has no mapping for the given key.
     */
    public V put(Object key, Object value) {
        throw new NotYetImplementedException();
    }

    /**
     * Copies all the mappings in the given map to this map. These mappings will
     * replace all mappings that this map had for all of the keys currently in
     * the given map.
     * 
     * @param map
     *            the key whose presence in this map is to be tested
     * @throws NullPointerException
     *             if the given map is null, or if one or more keys in the given
     *             map are null
     */
    public void putAll(Map<? extends K, ? extends V> map) {
        throw new NotYetImplementedException();
    }

    /**
     * Removes the mapping for this key from this map if it is present.
     * 
     * @param key
     *            the key whose mapping is to be removed from this map
     * @return the previous value associated with the given key, or null if this
     *         map has no mapping for this key.
     */
    public V remove(Object key) {
        throw new NotYetImplementedException();
    }

    /**
     * Answers the number of the mappings in this map.
     * 
     * @return the number of the mappings in this map
     */
    public int size() {
        throw new NotYetImplementedException();
    }

    /**
     * Answers a {@link Collection}} view of the values contained in this map.
     * The returned collection complys with the general rule specified in
     * {@link Map#values()}}. The collection's iterator will return the values
     * in the their corresponding keys' natural order(the enum constants are
     * declared in this order)
     * 
     * @return a collection view of the mappings contained in this map.
     */
    public Collection<V> values() {
        throw new NotYetImplementedException();
    }

}
