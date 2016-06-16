--TEST--
Testing __debugInfo() magic method with bad returns TRUE
--FILE--
<?php

class C
{
    public $val;
    public function __debugInfo()
    {
        return $this->val;
    }
    public function __construct($val)
    {
        $this->val = $val;
    }
}
function fn730702219()
{
    $c = new C(true);
    var_dump($c);
}
fn730702219();
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-true.php on line %d
