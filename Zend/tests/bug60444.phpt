--TEST--
Bug #60444 (Segmentation fault with include & class extending)
--FILE--
<?php

class Foo
{
    public function __construct()
    {
        eval("class Bar extends Foo {}");
        Some::foo($this);
    }
}
class Some
{
    public static function foo(Foo $foo)
    {
    }
}
function fn1946843160()
{
    new Foo();
    echo "done\n";
}
fn1946843160();
--EXPECT--
done
