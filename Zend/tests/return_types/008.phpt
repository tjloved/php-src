--TEST--
Return type covariance in interface implementation

--FILE--
<?php

interface foo
{
    public function bar() : foo;
}
class qux implements foo
{
    public function bar() : qux
    {
        return $this;
    }
}
function fn1034172885()
{
    $qux = new qux();
    var_dump($qux->bar());
}
fn1034172885();
--EXPECTF--
Fatal error: Declaration of qux::bar(): qux must be compatible with foo::bar(): foo in %s008.php on line %d
