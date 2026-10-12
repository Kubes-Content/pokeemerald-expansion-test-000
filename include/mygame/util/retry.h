//
// Created by kubes on 10/12/26.
//
#ifndef GUARD_KUBES_RETRY_H
#define GUARD_KUBES_RETRY_H

#define RETRY_CALL_BASE(maxAttempts, logicBeforePredicate, predicate, logicOnPredicateSuccess) \
    for (u8 attempt = 0; attempt < maxAttempts; attempt++) \
    { \
        logicBeforePredicate; \
        \
        if (!(predicate)) continue; \
        \
        logicOnPredicateSuccess; \
        break; \
    }

#define RETRY_CALL(maxAttempts, logicBeforePredicate, predicate) \
{ \
    RETRY_CALL_BASE(maxAttempts, logicBeforePredicate, predicate,) \
}

// ... are fatalf's args
#define RETRY_CALL_FATAL(maxAttempts, logicBeforePredicate, predicate, ...) \
{ \
    bool8 found = FALSE; \
    RETRY_CALL_BASE(maxAttempts, logicBeforePredicate, predicate, found = TRUE) \
    if (!found) fatalf(__VA_ARGS__); \
}

#endif // GUARD_KUBES_RETRY_H
