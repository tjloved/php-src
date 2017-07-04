--TEST--
Bug #69467 (Wrong checked for the interface by using Trait)
--FILE--
<?php

interface Baz
{
    public function bad();
}
trait Bar
{
    protected function bad()
    {
    }
}
class Foo implements Baz
{
    use Bar;
}
function fn1706316205()
{
    $test = new Foo();
    var_dump($test instanceof Baz);
}
fn1706316205();
--EXPECTF--
Fatal error: Access level to Foo::bad() must be public (as in class Baz) in %sbug69467.php on line %d
