--TEST--
ZE2 interfaces
--FILE--
<?php

interface ThrowableInterface
{
    public function getMessage();
}
class Exception_foo implements ThrowableInterface
{
    public $foo = "foo";
    public function getMessage()
    {
        return $this->foo;
    }
}
function fn989152975()
{
    $foo = new Exception_foo();
    echo $foo->getMessage() . "\n";
}
fn989152975();
--EXPECT--
foo

