## wieso : für division?

weil ich ein komma für dezimalzahlen verwenden will, musste irgendein anderes
zeichen für die trennung von z. B. argumenten verwendet werden:

    funktion(a, b, c)

würde nicht mehr funktionieren, da ein argument auch eine zahl sein kann

    funktion(34, 56, 78)

deshalb wird an dieser stelle ein strichpunkt ';' eingesetzt

    funktion (34; 56; 78)

da nun der strichpunkt für die trennung eingesetzt wird, brauche ich ein zeichen um eine anweisung abzuschließen.
habe mich für punkt '.' entschieden

    funktion (34; 56; 78).

somit kann der punkt nun nicht mehr für den zugriff auf die eigentschaft einer schablone verwendet werden:

    vec2.x

also, wird hier ein weiterer ersatz benötigt. hier habe ich mich für die pfadangabe entschieden und das '/' zeichen

    vec2/x

deshalb kann das pisa zeichen nicht mehr für die division verwendet werden. deshalb wird der doppelpunkt für die division verwendet!

    10 : 3

jetzt können wir aber den doppelpunkt nicht mehr ohne weiteres in der deklaration einsetzen

    a : d32 = 10,5.

denn sonst würde hier der eingelesene ausdruck nicht mehr BEZEICHNER('a') sein, sondern BINÄR('a' ':' 'd32'). also muss für die deklaration ein anderes zeichen verwendet werden. habe mich für den balken '|' entschieden.

    a | d32 = 10,5.

nun steht mir das zeichen nicht mehr für die bit-oder und logisches-oder operationen zur verfügung 😠

## Beispiele

    ## dezimalzahl
    3,14

    ## abschluss einer anweisung
    funktion().

    ## auflistung von argumenten
    funktion(a; b; 3,14; c).

    ## zugriff auf eine eigenschaft
    a/breite

    ## division
    10 : 3

    ## deklaration
    a | Vektor2 = { .x = 30,5 }.

    ## text
    »dies ist ein text«

    ## ganzzahlen mit basisangabe
    10b235 # basis 10
    2b0101_1111_0000
    16bFF99_AA33

