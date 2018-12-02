# Swerve Programming Language

Simple, expressive, correct and composable

## Spec

### Hello World
```
from io import print

print("Hello World!")
```

### Variable Assignment

With inferred types:
```
a = 1
b = "string"
c = True
d = 3.14
e = :keyword
f = [1, 2, 3]
g = {1, 2, 3}
h = {"a": 1, "b": 2}
```

With explicit types:
```
a2: Num = 1
b2: Str = "string"
c2: Float = 3.14
e2: Keyword = :keyword
f2: Seq[Num] = [1, 2, 3]
g2: Set[Num] = {1, 2, 3}
h2: Map[Str, Num] = {"a": 1, "b": 2}
```

Multi-line data literals
```
m1 = [
    1,
    2,
    3,
    4,
]
m2: Seq[Num] = [
    1,
    2,
    3,
    4,
]

m3 = {
    1,
    2,
    3,
    4,
}

m4: Set[Num] = {
    1,
    2,
    3,
    4,
}

m5 = {
    "a": 1,
    "b": 2,
    "c": 3,
}

m6: Map[Str, Num] = {
    "a": 1,
    "b": 2,
    "c": 3,
}
```

### Record Definition
```
data User:
    name: Str
    username: Str
    password: Str
```

### Record Construction

Inline construction:
```
user = User { "Joseph", "jmoniz", "123" }
```

Multi-line construction:
```
user = User {
    "Joseph",
    "jmoniz",
    "123",
}
```

Named inline construction:
```
user = User { name: "Joseph", username: "jmoniz", password: "123" }
```

Named multi-line construction
```
user = User {
    name: "Joseph",
    username: "jmoniz",
    password: 123,
}
```

### Enumeration Definition

Singleton definition
```
enum Bool:
    True
    False
```

Parameterized definition
```
A = TypeVar("A")
enum Optional:
    None
    Some(A)
```

Recursive definition:
```
enum Json:
    JsString(value: Str)
    JsBool(value: Bool)
    JsNumber(value: Num)
    JsArray(value: Seq[Json])
    JsObject(value: Map[Str, Json])
    JsNull
```

### Enumeration Construction

Inline construction:
```
value = Some("exists")
```

Multi-line construction
```
value = Some(
    "exists",
)
```

### If Expressions

Multi-line if expression
```
value = if boolean_value:
    "is_true"
else:
    "is_false"
```

Inline if expression
```
value = "is_true" if boolean_value else "is_false"
```


### If Statements

Multi-line if statement:
```
if boolean_value:
    return True
else:
    return False
```

Non total if statement (early return):
```
if some_error_check:
    return False
```

Multiple condition if statement:
```
if boolean_value:
    return True
elif other_boolean_value:
    return True
else:
    return False
```

### Function Definition

Inline parameters:
```
def add(a: Num, b: Num) -> Num:
    return a + b
```

Multi-line parameters:
```
def add(
    a: Num,
    b: Num,
) -> Num:
    42
```

Using type parameters:
```
A = TypeVar("A")
def len(iterable: Iterable[A]) -> Num:
    return sum(1 for _ in iterable)
```

### Function Invocation

Inline invocation:
```
doubled = double(42)
```

Multi-line invocation:
```
trippled = triple(
    42,
)
```

Named inline invocation: 
```
halved = halve(number = 42)
```

Named multiline invocation:
```
got = default_on_none(
    optional = None,
    default = 42,
)
```
