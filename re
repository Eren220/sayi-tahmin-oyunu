<!DOCTYPE html>
<html lang="tr">
<head>
  <meta charset="UTF-8">
  <title>Sayı Tahmin Oyunu</title>
  <style>
    body { font-family: sans-serif; text-align: center; margin: 20px; }
    .hidden { display: none; }
  </style>
</head>
<body>
  <div id="girisEkrani">
    <h1>Giriş Yap</h1>
    <input type="text" id="oyuncuAdi" placeholder="Adını yaz"><br>
    <input type="password" id="oyuncuSifre" placeholder="Şifren"><br>
    <select id="karakterSecimi">
      <option value="🦸‍♂️">Kahraman</option>
      <option value="🧙‍♀️">Büyücü</option>
      <option value="🕵️‍♂️">Ajan</option>
    </select><br>
    <select id="zorlukSecimi">
      <option value="kolay">Kolay (1-50)</option>
      <option value="orta">Orta (1-100)</option>
      <option value="zor">Zor (1-200)</option>
      <option value="imkansiz">İmkansız (1-500)</option>
    </select><br><br>
    <button onclick="girisYap()">Giriş Yap ve Başla</button>
  </div>

  <div id="oyunEkrani" class="hidden">
    <h1>Sayı Tahmin Etme Oyunu</h1>
    <p id="oyunBilgi"></p>
    <input type="number" id="tahmin" placeholder="Tahminini gir">
    <button onclick="tahminEt()">Tahmin Et</button>
    <p id="sonuc"></p>
    <p id="sicaklik"></p>
    <p id="hak"></p>
    <p id="skor"></p>

    <h2>Liderlik Tablosu</h2>
    <ol id="liderlikListesi"></ol>

    <h2>Admin Paneli</h2>
    <input type="password" id="adminSifre" placeholder="Admin şifresi">
    <button onclick="sifirlaLiderlik()">Liderliği Sıfırla</button>
  </div>

  <audio id="dogruSes" src="https://www.soundjay.com/buttons/sounds/button-3.mp3"></audio>
  <audio id="yanlisSes" src="https://www.soundjay.com/buttons/sounds/button-10.mp3"></audio>

  <script>
    let oyuncu = {}, kalanHak, tutulanSayi, maxSayi, oncekiFark = null, oyunBitti = false;
    let liderlikListesi = JSON.parse(localStorage.getItem("liderlik")) || [];
    const oyunPuaniBirimi = "💰 Altın";
    const adminSifre = "admin123";

    function girisYap() {
      const ad = document.getElementById("oyuncuAdi").value.trim();
      const sifre = document.getElementById("oyuncuSifre").value.trim();
      const karakter = document.getElementById("karakterSecimi").value;
      const zorluk = document.getElementById("zorlukSecimi").value;

      if (!ad || !sifre) return alert("Ad ve şifre zorunlu!");
      oyuncu = { ad, sifre, karakter, zorluk };

      switch (zorluk) {
        case "kolay": maxSayi = 50; break;
        case "orta": maxSayi = 100; break;
        case "zor": maxSayi = 200; break;
        case "imkansiz": maxSayi = 500; break;
      }

      kalanHak = 10;
      tutulanSayi = Math.floor(Math.random() * maxSayi) + 1;
      document.getElementById("girisEkrani").classList.add("hidden");
      document.getElementById("oyunEkrani").classList.remove("hidden");
      document.getElementById("oyunBilgi").innerText = `${karakter} ${ad} (${zorluk.toUpperCase()} mod - 1-${maxSayi})`;
      guncelleLiderlikTablosu();
    }

    function tahminEt() {
      if (oyunBitti) return;
      const tahmin = parseInt(document.getElementById("tahmin").value);
      const sonuc = document.getElementById("sonuc");
      const sicaklik = document.getElementById("sicaklik");
      const hak = document.getElementById("hak");
      const skor = document.getElementById("skor");

      if (isNaN(tahmin) || tahmin < 1 || tahmin > maxSayi) {
        sonuc.innerText = `1 ile ${maxSayi} arasında bir sayı girmelisin!`;
        return;
      }

      kalanHak--;
      const fark = Math.abs(tutulanSayi - tahmin);

      if (tahmin === tutulanSayi) {
        document.getElementById("dogruSes").play();
        const puan = kalanHak * 10;
        sonuc.innerText = `🎉 Doğru tahmin! Sayı: ${tutulanSayi}`;
        skor.innerText = `🏆 Skorun: ${puan} ${oyunPuaniBirimi}`;
        let kisi = liderlikListesi.find(k => k.ad === oyuncu.ad && k.sifre === oyuncu.sifre);
        if (kisi) {
          if (puan > kisi.skor) kisi.skor = puan;
        } else {
          liderlikListesi.push({ ad: oyuncu.ad, skor: puan, sifre: oyuncu.sifre });
        }
        localStorage.setItem("liderlik", JSON.stringify(liderlikListesi));
        oyunBitti = true;
        guncelleLiderlikTablosu();
        sicaklik.innerText = "";
      } else {
        document.getElementById("yanlisSes").play();
        sonuc.innerText = tahmin < tutulanSayi ? "Daha büyük dene." : "Daha küçük dene.";
        sicaklik.innerText = oncekiFark === null ? (fark <= 10 ? "🔥 Sıcağa yakınsın!" : "❄️ Soğuk tahmin!") :
          (fark < oncekiFark ? "📈 Daha sıcak!" : fark > oncekiFark ? "📉 Daha soğuk!" : "😐 Aynı sıcaklık.");
        oncekiFark = fark;
      }

      hak.innerText = `Kalan hak: ${kalanHak}`;
      if (kalanHak === 0) {
        sonuc.innerText = `😞 Hakkın bitti! Sayı: ${tutulanSayi}`;
        skor.innerText = `🏆 Skorun: 0 ${oyunPuaniBirimi}`;
        oyunBitti = true;
        guncelleLiderlikTablosu();
      }
    }

    function guncelleLiderlikTablosu() {
      const liste = document.getElementById("liderlikListesi");
      liste.innerHTML = "";
      liderlikListesi.sort((a, b) => b.skor - a.skor);
      liderlikListesi.slice(0, 5).forEach(kisi => {
        const sifreIcon = kisi.sifre ? "🔒" : "👤";
        const li = document.createElement("li");
        li.innerText = `${sifreIcon} ${kisi.ad}: ${kisi.skor} ${oyunPuaniBirimi}`;
        liste.appendChild(li);
      });
    }

    function sifirlaLiderlik() {
      const sifre = document.getElementById("adminSifre").value;
      if (sifre === adminSifre) {
        if (confirm("Liderlik tablosu sıfırlansın mı?")) {
          localStorage.removeItem("liderlik");
          liderlikListesi = [];
          guncelleLiderlikTablosu();
          alert("Sıfırlandı.");
        }
      } else {
        alert("Hatalı admin şifresi!");
      }
    }
  </script>
</body>
</html>
