package org.selfbus.updater.devicemgnt;

import com.google.common.collect.Lists;
import tuwien.auto.calimero.knxnetip.KNXnetIPTunnel;

import javax.annotation.Nonnull;
import javax.annotation.Nullable;
import java.lang.reflect.Field;
import java.util.List;

public class ReflectKNXnetIPTunnel {

    private KNXnetIPTunnel tunnel = null;
    Field fieldSeqNumb = null;
    private final static int SEQUENCE_NUMBER_INVALID = -1;
    int sequenceNumberSent = SEQUENCE_NUMBER_INVALID;

    @SuppressWarnings("unused")
    private ReflectKNXnetIPTunnel() {
    }

    public ReflectKNXnetIPTunnel(Object obj) {
        Field fieldConn = getField(obj, "conn");
        if (fieldConn == null) {
            return;
        }

        fieldConn.setAccessible(true);
        try {
            Object connValue = fieldConn.get(obj);
            if (!(connValue instanceof KNXnetIPTunnel)) {
                return;
            }
            tunnel = (KNXnetIPTunnel) connValue;
            fieldSeqNumb = getField(tunnel, "seqSend");
            if (fieldSeqNumb == null) {
                return;
            }
            fieldSeqNumb.setAccessible(true);
        }
        catch (IllegalAccessException ignored) {
        }
    }

    public int getSequenceNumberSent() {
        if (fieldSeqNumb == null) {
            return SEQUENCE_NUMBER_INVALID;
        }

        try {
            sequenceNumberSent = fieldSeqNumb.getInt(tunnel);
        }
        catch (IllegalAccessException ignored) {
            return SEQUENCE_NUMBER_INVALID;
        }
        return sequenceNumberSent;
    }

    private Field getField(Object obj, String fieldName) {
        // Iterate over all declared fields
        for (Field field : getFieldsUpTo(obj.getClass(), Object.class)) {
            if (field.getName().equals(fieldName)) {
                return field;
            }
        }
        return null;
    }

    private static Iterable<Field> getFieldsUpTo(@Nonnull Class<?> startClass,
                                                 @Nullable Class<?> exclusiveParent) {
        List<Field> currentClassFields = Lists.newArrayList(startClass.getDeclaredFields());
        Class<?> parentClass = startClass.getSuperclass();

        if (parentClass != null && (!(parentClass.equals(exclusiveParent)))) {
            List<Field> parentClassFields =
                    (List<Field>) getFieldsUpTo(parentClass, exclusiveParent);
            currentClassFields.addAll(parentClassFields);
        }
        return currentClassFields;
    }
}
