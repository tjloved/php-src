--TEST--
ZE2 A derived class does not know about privates of ancestors
--FILE--
<?php

class Bar
{
    public static function pub()
    {
        Bar::priv();
    }
    private static function priv()
    {
        echo "Bar::priv()\n";
    }
}
class Foo extends Bar
{
    public static function priv()
    {
        echo "Foo::priv()\n";
    }
}
function fn504953836()
{
    Foo::pub();
    Foo::priv();
    echo "Done\n";
}
fn504953836();
--EXPECTF--
Bar::priv()
Foo::priv()
Done
