--TEST--
Exception inside a foreach loop with return
--FILE--
<?php

class saboteurTestController
{
    public function isConsistent()
    {
        throw new \Exception();
    }
}
function fn671885208()
{
    $controllers = array(new saboteurTestController(), new saboteurTestController());
    foreach ($controllers as $controller) {
        try {
            if ($controller->isConsistent()) {
                return $controller;
            }
        } catch (\Exception $e) {
            echo "Exception\n";
        }
    }
}
fn671885208();
--EXPECT--
Exception
Exception
