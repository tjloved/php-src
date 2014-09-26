--TEST--
Exceptions before fatal error
--FILE--
<?php
function exception_error_handler($code, $msg) {
    throw new Exception($msg);
}

set_error_handler("exception_error_handler");

try {
    $foo->a();
} catch(Exception $e) {
    var_dump($e->getPrevious()->getMessage(), $e->getMessage());
}

try {
    new $foo();
} catch(Exception $e) {
    var_dump($e->getPrevious()->getMessage(), $e->getMessage());
}

try {
    throw $foo;
} catch(Exception $e) {
    var_dump($e->getPrevious()->getMessage(), $e->getMessage());
}

try {
    $foo();
} catch(Exception $e) {
    var_dump($e->getPrevious()->getMessage(), $e->getMessage());
}

try {
    $foo::b();
} catch(Exception $e) {
    var_dump($e->getPrevious()->getMessage(), $e->getMessage());
}


try {
    $b = clone $foo;
} catch(Exception $e) {
    var_dump($e->getPrevious()->getMessage(), $e->getMessage());
}

class b {
}

try {
    b::$foo();
} catch(Exception $e) {
    var_dump($e->getPrevious()->getMessage(), $e->getMessage());
}
?>
--EXPECT--
string(23) "Undefined variable: foo"
string(37) "Call to a member function a() on null"
string(23) "Undefined variable: foo"
string(45) "Class name must be a valid object or a string"
string(23) "Undefined variable: foo"
string(22) "Can only throw objects"
string(23) "Undefined variable: foo"
string(30) "Function name must be a string"
string(23) "Undefined variable: foo"
string(45) "Class name must be a valid object or a string"
string(23) "Undefined variable: foo"
string(35) "__clone method called on non-object"
string(23) "Undefined variable: foo"
string(30) "Function name must be a string"
