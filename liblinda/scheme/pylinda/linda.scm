(define-module (pylinda linda))

(load-extension "liblinda-guile.so" "scm_init_linda_module")

(Linda-connect (Linda-port))

(define out Linda-out)
(define in Linda-in)

(define uts (Linda-uts))

(define int-type (Linda-int-type))
(define string-type (Linda-string-type))

(define (make-type spec f1 f2) (let ((type (Linda-make-type spec f1 f2)))
                                    (lambda (x) (Linda-call-type type x))
                               )
)

(define toString Linda-toString)
(define toInt Linda-toInt)

(export uts out in int-type string-type make-type toString toInt)
