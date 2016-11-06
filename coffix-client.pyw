from tkinter import *
import socket

class Interface(Frame):

    def __init__(self, fenetre, **kwargs):

        Frame.__init__(self, fenetre, width = 100, height = 100, **kwargs)

        self.texte_adresse = Label(fenetre, text="Adresse : ")
        self.valeur_adresse = StringVar()
        self.champ_adresse = Entry(fenetre, textvariable=self.valeur_adresse, width=30)

        self.texte_port = Label(fenetre, text="Port : ")
        self.valeur_port = StringVar()
        self.champ_port = Entry(fenetre, textvariable=self.valeur_port, width=10)

        self.bouton_ok = Button(fenetre, text="Go !", command=self.coffee_go)

        self.texte_adresse.pack()
        self.champ_adresse.pack()
        self.texte_port.pack()
        self.champ_port.pack()
        self.bouton_ok.pack()

    def coffee_go(self):
        connexion = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connexion.connect((self.valeur_adresse.get(), eval(self.valeur_port.get())))
        request = "BREW HTCPCP/1.0";
        request = request.encode();
        connexion.send(request + b'\n')
        connexion.close()
        fenetre.destroy()

fenetre = Tk()
fenetre.title("Coffix")
fenetre.geometry("%dx%d%+d%+d" % (300, 120, 100, 100))

interface = Interface(fenetre)
interface.mainloop()
