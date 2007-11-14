package org.apache.harmony.jndi.provider.ldap.sasl;

import java.io.IOException;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Set;

import javax.naming.AuthenticationNotSupportedException;
import javax.naming.Context;
import javax.naming.ldap.Control;
import javax.security.auth.callback.Callback;
import javax.security.auth.callback.CallbackHandler;
import javax.security.auth.callback.NameCallback;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.UnsupportedCallbackException;
import javax.security.sasl.RealmCallback;
import javax.security.sasl.RealmChoiceCallback;
import javax.security.sasl.Sasl;
import javax.security.sasl.SaslClient;
import javax.security.sasl.SaslException;

import org.apache.harmony.jndi.provider.ldap.BindOp;
import org.apache.harmony.jndi.provider.ldap.LdapClient;
import org.apache.harmony.jndi.provider.ldap.LdapResult;
import org.apache.harmony.jndi.provider.ldap.asn1.Utils;
import org.apache.harmony.jndi.provider.ldap.parser.ParseException;

/**
 * A class used to perform SASL Bind Operation
 */
public class SaslBind {

    // provider supported sasl mechanisms
    public static final String DIGEST_MD5 = "DIGEST-MD5";

    public static final String CRAM_MD5 = "CRAM-MD5";

    public static final String GSSAPI = "GSSAPI";

    public static final String EXTERNAL = "EXTERNAL";

    private static Set<String> supportedSaslMechs = new HashSet<String>();

    static {
        supportedSaslMechs.add(DIGEST_MD5);
        supportedSaslMechs.add(CRAM_MD5);
        supportedSaslMechs.add(GSSAPI);
        supportedSaslMechs.add(EXTERNAL);
    }

    public enum AuthMech {
        None, Simple, SASL
    };

    private AuthMech authMech;

    private String saslMech;

    // -------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------

    public SaslBind() {

    }

    // -------------------------------------------------------------------
    // Methods
    // -------------------------------------------------------------------

    public AuthMech getAuthMech() {
        return this.authMech;
    }

    /**
     * Perform a LDAP SASL bind operation
     * 
     * @param env
     * @throws IOException
     * @throws AuthenticationNotSupportedException
     * @return int result code of bind operation, see RFC4411. -1 means
     *         authentication mechanisms in this binding operation is not SASL.
     * @throws ParseException 
     */
    public LdapResult doSaslBindOperation(Hashtable env, LdapClient client, Control[] controls)
            throws IOException, AuthenticationNotSupportedException {
        // This method only deals with SASL bind. It will return
        // immediatly if authentication mechanism is not SASL
        externalValueAuthMech(env);
        if (authMech == AuthMech.None || authMech == AuthMech.Simple) {
            return null;
        }

        // Initial server name for SaslClient
        String host = client.getAddress();

        // Initial CallbackHandler for SaslClient
        CallbackHandler cbh = (env.get("java.naming.security.sasl.callback") != null ? (CallbackHandler) env
                .get("java.naming.security.sasl.callback")
                : new DefaultCallbackHandler(env));

        // Initial authrization Id for SaslClient
        String authorizationId = "";
        if (env.get("java.naming.security.sasl.authorizationId") != null) {
            authorizationId = (String) env
                    .get("java.naming.security.sasl.authorizationId");
        } else {
            authorizationId = (String) env.get(Context.SECURITY_PRINCIPAL);
        }

        // Create SASL client to use for authentication
        SaslClient saslClnt = Sasl.createSaslClient(new String[] { saslMech },
                authorizationId, "ldap", host, env, cbh);

        if (saslClnt == null) {
            throw new SaslException("SASL client not available");
        }

        // If the specific mechanism needs initial response, get one
        byte[] response = (saslClnt.hasInitialResponse() ? saslClnt
                .evaluateChallenge(new byte[0]) : null);

        // do bind operation, including the initial
        // response (if any)
        BindOp bind = new BindOp("", "", saslMech, response);
        client.doOperation(bind, controls);
        LdapResult res = bind.getResult();

        // If DefaultCallbackHandler is used, DIGEST-MD5 needs realm in
        // callbacke handler
        if (DIGEST_MD5.equals(saslMech)
                && cbh instanceof DefaultCallbackHandler) {
            ((DefaultCallbackHandler) cbh).setRealm(getRealm(new String(bind
                    .getServerSaslCreds())));
        }

        // Authentication done?
        while (!saslClnt.isComplete()
                && (res.getResultCode() == LdapResult.SASL_BIND_IN_PROGRESS || res
                        .getResultCode() == LdapResult.SUCCESS)) {

            // No, process challenge to get an appropriate next
            // response
            byte[] challenge = bind.getServerSaslCreds();
            response = saslClnt.evaluateChallenge(challenge);

            // May be a success message with no further response
            if (res.getResultCode() == LdapResult.SUCCESS) {

                if (response != null) {
                    // Protocol error; supposed to be done already
                    throw new SaslException("Protocol error in "
                            + "SASL session");
                }
                System.out.println("success");
                break; // done
            }

            // Wrap the response in another bind request and send
            // it off
            bind.setSaslCredentials(response);
            client.doOperation(bind, controls);
            res = bind.getResult();
        }

        return bind.getResult();
    }

    public AuthMech valueAuthMech(Hashtable env)
            throws AuthenticationNotSupportedException {
        return externalValueAuthMech(env);
    }

    private AuthMech externalValueAuthMech(Hashtable env)
            throws AuthenticationNotSupportedException {
        if (env == null) {
            // FIXME: handle exception here?
            return null;
        }

        if (env.get(Context.SECURITY_AUTHENTICATION) == null) {
            if (env.get(Context.SECURITY_PRINCIPAL) == null) {
                this.authMech = AuthMech.None;
            } else {
                this.authMech = AuthMech.Simple;
            }
        } else if (((String) env.get(Context.SECURITY_AUTHENTICATION))
                .equalsIgnoreCase("none")) {
            this.authMech = AuthMech.None;
        } else if (((String) env.get(Context.SECURITY_AUTHENTICATION))
                .equalsIgnoreCase("simple")) {
            this.authMech = AuthMech.Simple;
        } else if (valueSaslMech((String) env
                .get(Context.SECURITY_AUTHENTICATION))) {
            this.authMech = AuthMech.SASL;
        } else {
            throw new AuthenticationNotSupportedException((String) env
                    .get(Context.SECURITY_AUTHENTICATION));
        }

        return this.authMech;
    }

    /**
     * Value if those mechanisms in the string are supported
     * 
     * @param auth
     *            a space separated string of sasl mechanisms
     * @return
     */
    private boolean valueSaslMech(String auth) {
        boolean flag = false;
        String[] saslMechs = auth.trim().split(" ");

        for (int i = 0; i < saslMechs.length; i++) {
            if (saslMechs != null && saslMechs[i] != "") {
                if (supportedSaslMechs.contains(saslMechs[i])) {
                    flag = true;
                    saslMech = saslMechs[i];
                    break;
                }
            }
        }
        return flag;
    }

    private String getRealm(String creds) {
        String[] credsProps = creds.split(",");
        for (int i = 0; i < credsProps.length; i++) {
            if (credsProps[i].startsWith("realm")) {
                System.out.println(credsProps[i]);
                String realm = credsProps[i].substring(7, credsProps[i]
                        .length() - 1);
                System.out.println(realm);
                return realm;
            }
        }
        return "";
    }
}

/*
 * Default callback handler, may be customized through
 * "java.naming.security.sasl.realm".
 */
class DefaultCallbackHandler implements CallbackHandler {
    
    private static final String JAVA_NAMING_SECURITY_SASL_REALM = "java.naming.security.sasl.realm";

    private Hashtable env;

    private String realm = "";

    public DefaultCallbackHandler() {

    }

    public DefaultCallbackHandler(Hashtable env) {
        this.env = env;
    }

    public void handle(Callback[] callbacks) throws java.io.IOException,
            UnsupportedCallbackException {
        for (int i = 0; i < callbacks.length; i++) {
            if (callbacks[i] instanceof RealmChoiceCallback) {
                // TODO what to do here?
                // RealmChoiceCallback rcc = (RealmChoiceCallback) callbacks[i];

            } else if (callbacks[i] instanceof RealmCallback) {
                RealmCallback rc = (RealmCallback) callbacks[i];
                if (env.get(JAVA_NAMING_SECURITY_SASL_REALM) != null) {
                    realm = (String) env.get(JAVA_NAMING_SECURITY_SASL_REALM);
                    rc.setText(realm);
                } else {
                    rc.setText(realm);
                }
            } else if (callbacks[i] instanceof PasswordCallback) {
                PasswordCallback pc = (PasswordCallback) callbacks[i];
                pc.setPassword(Utils.getCharArray(env
                        .get(Context.SECURITY_CREDENTIALS)));
            } else if (callbacks[i] instanceof NameCallback) {
                //authentication Id
                NameCallback nc = (NameCallback) callbacks[i];
                nc.setName((String) env.get(Context.SECURITY_PRINCIPAL));
            } else {
                throw new UnsupportedCallbackException(callbacks[i]);
            }
        }
    }

    public void setRealm(String realm) {
        this.realm = realm;
    }
}
