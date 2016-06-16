--TEST--
Testing indirect method call and exceptions
--FILE--
<?php

class foo
{
    public function __construct()
    {
        throw new Exception('foobar');
    }
}
function fn55455429()
{
    try {
        $X = (new foo())->Inexistent(3);
    } catch (Exception $e) {
        var_dump($e->getMessage());
        // foobar
    
    }
}
fn55455429();
--EXPECT--
string(6) "foobar"
