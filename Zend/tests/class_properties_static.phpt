--TEST--
Static Class Property Expressions
--FILE--
<?php

class Foo
{
    public $b1 = 1 + 1;
    public $b2 = 1 << 2;
    public $b3 = "foo " . " bar " . " baz";
}
function fn538243247()
{
    $f = new Foo();
    var_dump($f->b1, $f->b2, $f->b3);
}
fn538243247();
--EXPECT--
int(2)
int(4)
string(13) "foo  bar  baz"
