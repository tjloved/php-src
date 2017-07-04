--TEST--
Internal covariant return type of self

--FILE--
<?php

class Foo
{
    public static function test() : self
    {
        return new Foo();
    }
}
class Bar extends Foo
{
    public static function test() : parent
    {
        return new Bar();
    }
}
function fn10337552()
{
    var_dump(Bar::test());
    var_dump(Foo::test());
}
fn10337552();
--EXPECTF--
object(Bar)#%d (0) {
}
object(Foo)#%d (0) {
}
