--TEST--
Ensure type hints are enforced for functions invoked as callbacks.
--FILE--
<?php
function try_call_user_func($fn, ...$args) {
    try {
        call_user_func_array($fn, $args);
    } catch (EngineException $e) {
        echo $e->getMessage(), "\n";
    }
}

echo "---> Type hints with callback function:\n";
class A  {  }
function f1(A $a)  {
    echo "in f1;\n";
}
function f2(A $a = null)  {
    echo "in f2;\n";
}
try_call_user_func('f1', 1);
try_call_user_func('f1', new A);
try_call_user_func('f2', 1);
try_call_user_func('f2');
try_call_user_func('f2', new A);
try_call_user_func('f2', null);

echo "\n\n---> Type hints with callback static method:\n";
class C {
    static function f1(A $a) {
        if (isset($this)) {
            echo "in C::f1 (instance);\n";
        } else {
            echo "in C::f1 (static);\n";
        }
    }
    static function f2(A $a = null) {
        if (isset($this)) {
            echo "in C::f2 (instance);\n";
        } else {
            echo "in C::f2 (static);\n";
        }
    }
}
try_call_user_func(array('C', 'f1'), 1);
try_call_user_func(array('C', 'f1'), new A);
try_call_user_func(array('C', 'f2'), 1);
try_call_user_func(array('C', 'f2'));
try_call_user_func(array('C', 'f2'), new A);
try_call_user_func(array('C', 'f2'), null);


echo "\n\n---> Type hints with callback instance method:\n";
class D {
    function f1(A $a) {
        if (isset($this)) {
            echo "in C::f1 (instance);\n";
        } else {
            echo "in C::f1 (static);\n";
        }
    }
    function f2(A $a = null) {
        if (isset($this)) {
            echo "in C::f2 (instance);\n";
        } else {
            echo "in C::f2 (static);\n";
        }
    }
}
$d = new D;
try_call_user_func(array($d, 'f1'), 1);
try_call_user_func(array($d, 'f1'), new A);
try_call_user_func(array($d, 'f2'), 1);
try_call_user_func(array($d, 'f2'));
try_call_user_func(array($d, 'f2'), new A);
try_call_user_func(array($d, 'f2'), null);

?>
--EXPECTF--
---> Type hints with callback function:
Argument 1 passed to f1() must be an instance of A, integer given, called in %s on line %d and defined
in f1;
Argument 1 passed to f2() must be an instance of A, integer given, called in %s on line %d and defined
in f2;
in f2;
in f2;


---> Type hints with callback static method:
Argument 1 passed to C::f1() must be an instance of A, integer given, called in %s on line %d and defined
in C::f1 (static);
Argument 1 passed to C::f2() must be an instance of A, integer given, called in %s on line %d and defined
in C::f2 (static);
in C::f2 (static);
in C::f2 (static);


---> Type hints with callback instance method:
Argument 1 passed to D::f1() must be an instance of A, integer given, called in %s on line %d and defined
in C::f1 (instance);
Argument 1 passed to D::f2() must be an instance of A, integer given, called in %s on line %d and defined
in C::f2 (instance);
in C::f2 (instance);
in C::f2 (instance);
