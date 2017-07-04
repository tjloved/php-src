--TEST--
Exceptions before fatal error
--FILE--
<?php

function exception_error_handler($code, $msg)
{
    throw new Exception($msg);
}
class b
{
}
function fn2088363684()
{
    set_error_handler("exception_error_handler");
    try {
        $foo->a();
    } catch (Throwable $e) {
        var_dump($e->getMessage());
    }
    try {
        new $foo();
    } catch (Throwable $e) {
        var_dump($e->getMessage());
    }
    try {
        throw $foo;
    } catch (Throwable $e) {
        var_dump($e->getMessage());
    }
    try {
        $foo();
    } catch (Throwable $e) {
        var_dump($e->getMessage());
    }
    try {
        $foo::b();
    } catch (Throwable $e) {
        var_dump($e->getMessage());
    }
    try {
        $b = clone $foo;
    } catch (Throwable $e) {
        var_dump($e->getMessage());
    }
    try {
        b::$foo();
    } catch (Throwable $e) {
        var_dump($e->getMessage());
    }
}
fn2088363684();
--EXPECT--
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
string(23) "Undefined variable: foo"
