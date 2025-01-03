module eu.durdeuvladioan.frontendsscproject {
    requires javafx.controls;
    requires javafx.fxml;
    requires javafx.web;

    requires org.controlsfx.controls;
    requires com.dlsc.formsfx;
    requires net.synedra.validatorfx;
    requires org.kordamp.ikonli.javafx;
    requires org.kordamp.bootstrapfx.core;
    requires eu.hansolo.tilesfx;
    requires com.almasb.fxgl.all;
    requires com.fasterxml.jackson.databind;
    requires java.logging;
    requires javafx.swing;

    opens eu.durdeuvladioan.frontendsscproject to javafx.fxml;
    exports eu.durdeuvladioan.frontendsscproject;
}