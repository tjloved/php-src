--TEST--
ZE2 A derived class does not know about privates of ancestors
--FILE--
<?php

class Bar
{
    public function pub()
    {
        $this->priv();
    }
    private function priv()
    {
        echo "Bar::priv()\n";
    }
}
class Foo extends Bar
{
    public function priv()
    {
        echo "Foo::priv()\n";
    }
}
function fn1138497592()
{
    $obj = new Foo();
    $obj->pub();
    $obj->priv();
    echo "Done\n";
}
fn1138497592();
--EXPECTF--
Bar::priv()
Foo::priv()
Done
