--TEST--
Bug #21888 (protected property and protected method of the same name)
--FILE--
<?php

class mom
{
    protected $prot = "protected property\n";
    protected function prot()
    {
        print "protected method\n";
    }
}
class child extends mom
{
    public function callMom()
    {
        $this->prot();
    }
    public function viewMom()
    {
        print $this->prot;
    }
}
function fn1161243355()
{
    $c = new child();
    $c->callMom();
    $c->viewMom();
}
fn1161243355();
--EXPECT--
protected method
protected property
