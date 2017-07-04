--TEST--
Return type allows self

--FILE--
<?php

class Foo
{
    public static function getInstance() : self
    {
        return new static();
    }
}
class Bar extends Foo
{
}
function fn1642550194()
{
    var_dump(Foo::getInstance());
    var_dump(Bar::getInstance());
}
fn1642550194();
--EXPECTF--
object(Foo)#%d (%d) {
}
object(Bar)#%d (%d) {
}
